#include "sd_functions.h"
#include "capture_and_store.h"
#include "common_utils.h"

#define ENTRY_STR_SIZE 60
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
	uint16_t *voice_buf = pvPortMalloc(U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
	char *entry;
	if(voice_buf == NULL){
		*message = DISPLAY_ERROR_MEMORY;
		return false;
	}

	//Envio mensaje a display para indicar que comienza el reconocimiento de voz
	xSemaphoreGive(sd_display_sync);											//Cedo el semaforo para que lo tome la tarea display
	*message = DISPLAY_START_SPEECH_REC;
	xQueueSend(display_queue, message, portMAX_DELAY);							//Envio mensaje al display
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);								//Bloqueo la tarea hasta que el display me devuelva el semaforo

	capture_voice(voice_buf, AUDIO_BUFFER_SIZE);
	store_voice(voice_buf, AUDIO_BUFFER_SIZE, FRAME_SIZE, "current_voice.bin");
	vPortFree(voice_buf);

	//Extraigo los features de la voz
	entry = pvPortMalloc(CHAR_SIZE_BYTES(ENTRY_STR_SIZE));
	extract_and_save_features("current_voice.bin", "current_feature.bin");
	if(check_voice(template_path, "current_feature.bin")){
		build_entry_message(entry, user_name, "Concedido\n");
		write_entry("registro.txt", entry);
		*message = DISPLAY_VOICE_RECOGNIZED;
		vPortFree(entry);
		return true;
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
		write_entry("registro.txt", entry);
		*message = DISPLAY_VOICE_NOT_RECOGNIZED;
		return false;
	}
}
