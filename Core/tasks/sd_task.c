#include "sd_utils.h"
/*================[Private defines]========================*/
#define CODE_VERSION 1

/*================[Public task]==========================*/
#if CODE_VERSION == 1
void sd_task(void *pvParameters){
	//Variables usadas por la tarea SD
	display_message_t message;										//Para enviar mensajes al display
	FRESULT res;													//Para guardar los resultados de las operaciones de manejo de archivos
	char user_key_retrieved[SEQUENCE_LENGTH + 1];					//Para recibir los digitos desde el keypad
	char *template_path;											//Para guardar la direccion del template asociado al usuario
	char *user_name;												//Para guardar el nombre de usuario

	//Control inicial. Verifico existencia de archivos importantes
	if(!initialize_sd_and_verify_files(&message)){
		//No se pudieron realizar las inicializaciones
		xQueueSend(display_queue, &message, portMAX_DELAY);			//Envio pantalla de error al display
		while(1);													//Loop permanente. Resetear y solucionar el problema.
	}

	while(1){
		//Espero a que llegue la clave de usuario ingresada desde la tarea keypad
		for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
			xQueueReceive(sequence_queue, &user_key_retrieved[i], portMAX_DELAY);
		}
		xSemaphoreTake(keypad_sd_sync, portMAX_DELAY);												//Luego de recibir la secuencia, bloqueo la tarea keypad

		//Reservo memoria para almacenar usuario
		user_name = pvPortMalloc(USER_STR_SIZE * sizeof(char));
		if(user_name == NULL){
			message = DISPLAY_ERROR_MEMORY;
			xQueueSend(display_queue, &message, portMAX_DELAY);
			continue;
		}

		//Chequeo existencia de usuario
		if(!process_user_key(user_key_retrieved, user_name, &message)){
			//Usuario no existe. Enviar mensaje al display
			xQueueSend(display_queue, &message, portMAX_DELAY);
			vPortFree(user_name);
			continue;
		}

		//Indico que se encontro el usuario
		message = DISPLAY_USER_FOUND;
		xQueueSend(display_queue, &message, portMAX_DELAY);

		//Reservo memoria para armar el template path
		template_path = pvPortMalloc(TEMPLATE_STR_SIZE * sizeof(char));
		if(template_path == NULL){
			message = DISPLAY_ERROR_MEMORY;
			xQueueSend(display_queue, &message, portMAX_DELAY);
			vPortFree(user_name);
			continue;
		}

		//Verifico la existencia del template
		snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
		if(check_if_file_exists(template_path) != FR_OK){
			message = DISPLAY_TEMPLATE_NOT_FOUND;
			xQueueSend(display_queue, &message, portMAX_DELAY);
			vPortFree(user_name);
			vPortFree(template_path);
			continue;
		}

		//Realizo reconocimiento de voz
		if(recognize_user_voice(template_path, user_name, &message)){
			//Activo cerradura

		}

		//Libero memoria y envio mensaje el mensaje que corresponda al display
		vPortFree(user_name);
		vPortFree(template_path);

		xSemaphoreGive(sd_display_sync);
		xQueueSend(display_queue, &message, portMAX_DELAY);
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);												//Bloqueo la tarea hasta que termine de mostrarse el mensaje de reconocimiento
		xSemaphoreGive(keypad_sd_sync);																//Doy el semaforo para que la tarea keypad pueda ejecutarse
	}
}

#elif CODE_VERSION == 2
//Generador de template
void sd_task(void *pvParameters){
	indicatorMessage current_message;
	uint16_t *voice_buffer;
	char filename[NUM_OF_SAMPLES][15] = {"voice_1.bin", "voice_2.bin", "voice_3.bin", "voice_4.bin", "voice_5.bin"};
	//Monto la tarjeta SD. Si  esta correcto, se continua con el procesamiento.
	if(mount_sd("") == FR_OK){
		voice_buffer = pvPortMalloc(VOICE_BUFFER_SIZE * sizeof(uint16_t));
		for(uint8_t i = 0; i < NUM_OF_SAMPLES; i++){
			//Indico al display que se inicia el reconocimiento de voz
			xSemaphoreGive(sd_display_sync);												//Doy el semaforo para que el display pueda tomarlo
			current_message = PANTALLA_RECONOCIMIENTO_DE_VOZ;
			xQueueSend(display_queue, &current_message, portMAX_DELAY);						//Envio el evento de reconocimiento al display
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);									//Bloqueo la tarea hasta que el display me devuelva el semaforo

			//Capturo 1.5 segundos de voz
			capture_voice(voice_buffer, VOICE_BUFFER_SIZE);

			//Envio mensaje al display que indique que se estan procesando los datos
			current_message = PANTALLA_PROCESANDO_DATOS;
			xQueueSend(sd_display_sync, &current_message, portMAX_DELAY);					//En este caso puedo seguir procesando mientras se muestra el mensaje

			//Guardo la voz en la SD
			store_voice(voice_buffer, VOICE_BUFFER_SIZE, filename[i]);						//Guardo voz en el archivo voice_{i+1}.bin
		}
		vPortFree(voice_buffer);

		//Genero template
		generate_template();

		//Envio mensaje al display para indicarle que ya se guardo el template
		xSemaphoreGive(sd_display_sync);
		current_message = PANTALLA_TEMPLATE_GUARDADO;
		xQueueSend(display_queue, &current_message, portMAX_DELAY);
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);										//Bloqueo la tarea hasta que el display me devuelva el semaforo

		//Desmonto tarjeta SD y libero memoria ocupada con filename
		unmount_sd("");
	}

	while(1){

	}
}
#endif


