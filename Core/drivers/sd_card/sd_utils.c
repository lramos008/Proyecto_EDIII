#include "sd_functions.h"
#include "capture_and_store.h"
#include "common_utils.h"
#define ARM_MATH_CM4
#include "arm_math.h"

/*Este archivo contiene las funciones principales de la tarea SD*/
#define NO_DATABASE FR_NO_FILE
#define NO_REGISTER FR_DISK_ERR

FRESULT initialize_files(void){
	FRESULT res;

	//Chequeo que exista el archivo con la database de usuarios
	res = check_if_file_exists("usuarios.txt");
	if(res != FR_OK){
		return NO_DATABASE;
	}

	//Chequeo si existe el archivo de registro de accesos. Si no existe, lo creo
	res = check_if_file_exists("registro.txt");
	if(res != FR_OK){
		res = create_file("registro.txt", "Fecha Usuario Estado\n");
		if(res != FR_OK){
			return NO_REGISTER;
		}
	}
	return FR_OK;
}

bool initialize_sd_and_verify_files(display_message_t *error_message){
	FRESULT res;

	res = mount_sd("");
	if(res != FR_OK){
		*error_message = DISPLAY_ERROR_SD;
		return false;
	}

	res = initialize_files();
	if(res != FR_OK){
		switch(res){
		case NO_DATABASE:
			*error_message = DISPLAY_DATABASE_NOT_FOUND;
			break;
		case NO_REGISTER:
			*error_message = DISPLAY_REGISTER_NOT_CREATED;
			break;
		default:
			*error_message = DISPLAY_ERROR_UNKNOWN;
			break;
		}
		unmount_sd("");
		return false;
	}

	//No desmontar si se encontraron los archivos necesarios
	return true;
}

bool process_user_key(char *user_key, char *user_name, display_message_t *error_message){
	FRESULT res = search_user("usuarios.txt", user_key, user_name);
	if(res != FR_OK){
		*error_message = DISPLAY_USER_NOT_FOUND;
		return false;
	}
	clear_char(user_name, '\r');										//Limpio '\r' agregado por windows
	return true;
}

void build_entry_message(char *entry, char *user_name, const char *status){
	get_time_from_rtc(entry);						//Obtengo fecha y hora
	if(user_name != NULL){
		strcat(entry, user_name);					//Concateno nombre de usuario
	}
	else{
		strcat(entry, "Desconocido");				//Si no existe, concateno desconocido
	}
	strcat(entry, " ");
	strcat(entry, status);							//Concateno estado de acceso
	strcat(entry, "\n");
	return;
}

bool recognize_user_voice(char *template_path, char *user_name, display_message_t *message){
	size_t free_heap = xPortGetFreeHeapSize();
	uint16_t *voice_buf = pvPortMalloc(U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
	display_message_t aux;
	char *entry;
	if(voice_buf == NULL){
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}

	//Envio mensaje a display para indicar que comienza el reconocimiento de voz
	aux = DISPLAY_START_SPEECH_REC;
	xQueueSend(display_queue, &aux, portMAX_DELAY);									//Envio mensaje al display
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);									//Bloqueo la tarea hasta que el display me devuelva el semaforo


	capture_voice(voice_buf, AUDIO_BUFFER_SIZE);

	//Almaceno la voz en la SD
	aux = DISPLAY_PROCESSING_DATA;
	xQueueSend(display_queue, &aux, portMAX_DELAY);									//Envio mensaje indicando que se esta procesando la voz
	store_voice(voice_buf, AUDIO_BUFFER_SIZE, FRAME_SIZE, "current_voice.bin");
	vPortFree(voice_buf);
	free_heap = xPortGetFreeHeapSize();

	//Extraigo los features de la voz
	entry = pvPortMalloc(CHAR_SIZE_BYTES(ENTRY_STR_SIZE));
	if(entry == NULL){
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}
	extract_and_save_features("current_voice.bin", "current_feature.bin");
	if(check_voice(template_path, "current_feature.bin")){
		build_entry_message(entry, user_name, "Concedido\n");
		write_entry("registro.txt", entry);
		*message = DISPLAY_VOICE_RECOGNIZED;
		vPortFree(entry);
		f_unlink("current_feature.bin");
		return true;
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
		write_entry("registro.txt", entry);
		*message = DISPLAY_VOICE_NOT_RECOGNIZED;
		vPortFree(entry);
		f_unlink("current_feature.bin");
		return false;
	}
}

bool generate_template(void){
	display_message_t message;
	float *template, *my_feature;
	char filenames[NUM_OF_VOICES][20] = {"feature_1.bin", "feature_2.bin", "feature_3.bin", "feature_4.bin", "feature_5.bin"};


	//Reservo memoria para el buffer que contiene la captura de voz
	uint16_t *voice_buf = pvPortMalloc(U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
	if(voice_buf == NULL){
		//Manejar el error
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}
	//Capturo las voces y las guardo en la tarjeta SD
	for(uint8_t i = 0; i < NUM_OF_VOICES; i++){
		//Envio mensaje de inicio de reconocimiento al display
		message = DISPLAY_START_SPEECH_REC;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);							//Bloqueo la tarea hasta que el display me devuelva el semaforo

		//Capturo voces y las almaceno en la SD
		capture_voice(voice_buf, AUDIO_BUFFER_SIZE);
		message = DISPLAY_PROCESSING_DATA;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		store_voice(voice_buf, AUDIO_BUFFER_SIZE, FRAME_SIZE, "current_voice.bin");
		extract_and_save_features("current_voice.bin", filenames[i]);											//Borro el archivo current_voice.bin
	}

	//Libero memoria utilizada para capturar voz
	vPortFree(voice_buf);

	//Reservo memoria utilizada para crear el template
	template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	my_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(template == NULL || my_feature == NULL){
		//Manejar error
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}

	arm_fill_f32(0.0f, template, FEATURE_SIZE);

	for(uint8_t i = 0; i < NUM_OF_FRAMES; i++){
		//En cada iteracion se procesa el bloque i del feature_{j+1}.bin
		for(uint8_t j = 0; j < NUM_OF_VOICES; j++){
			read_buffer_from_sd(filenames[j], my_feature, FEATURE_SIZE, i * FEATURE_SIZE);
			arm_add_f32(template, my_feature, template, FEATURE_SIZE);
		}

		//Escalo para obtener el promedio
		arm_scale_f32(template, 1.0f / NUM_OF_VOICES, template, FEATURE_SIZE);
		save_buffer_on_sd("current_template.bin", template, FEATURE_SIZE);
		arm_fill_f32(0.0f, template, FEATURE_SIZE);
	}

	//Borro los archivos generados para crear el template
	for(uint8_t i = 0; i < NUM_OF_VOICES; i++){
		f_unlink(filenames[i]);
	}

	//Libero memoria de template y my_feature
	vPortFree(template);
	vPortFree(my_feature);
	return true;
}
