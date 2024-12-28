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

		//Verifico si la clave es 000000. En cuyo caso se debe crear el template
		if(strcmp(user_key_retrieved, "000000") == 0){
			if(generate_template()){
				//Muestro mensaje en display indicando que se creo el template
				message = DISPLAY_TEMPLATE_SAVED;
				xQueueSend(display_queue, &message, portMAX_DELAY);
				xSemaphoreTake(sd_display_sync, portMAX_DELAY);
				xSemaphoreGive(keypad_sd_sync);
			}
			else{
				//Mostrar mensaje no se pudo generar template
				message = DISPLAY_TEMPLATE_NOT_FOUND;
				xQueueSend(display_queue, &message, portMAX_DELAY);
				xSemaphoreTake(sd_display_sync, portMAX_DELAY);
				xSemaphoreGive(keypad_sd_sync);
			}
			continue;
		}

		//Continuo con flujo normal de ejecucion

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
			xSemaphoreGive(keypad_sd_sync);
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
			xSemaphoreGive(keypad_sd_sync);
			continue;
		}

		//Verifico la existencia del template
		snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
		if(check_if_file_exists(template_path) != FR_OK){
			message = DISPLAY_TEMPLATE_NOT_FOUND;
			xQueueSend(display_queue, &message, portMAX_DELAY);
			vPortFree(user_name);
			vPortFree(template_path);
			xSemaphoreGive(keypad_sd_sync);
			continue;
		}

		//Realizo reconocimiento de voz
		if(recognize_user_voice(template_path, user_name, &message)){
			//Activo cerradura

		}

		//Libero memoria y envio mensaje el mensaje que corresponda al display
		vPortFree(user_name);
		vPortFree(template_path);

		xQueueSend(display_queue, &message, portMAX_DELAY);
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);												//Bloqueo la tarea hasta que termine de mostrarse el mensaje de reconocimiento
		xSemaphoreGive(keypad_sd_sync);																//Doy el semaforo para que la tarea keypad pueda ejecutarse
	}
}

#endif


