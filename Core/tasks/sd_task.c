#include "sd_utils.h"
/*================[Private defines]========================*/
#define CODE_VERSION 1

/*================[Public task]==========================*/
#if CODE_VERSION == 1
void sd_task(void *pvParameters){
	//Variables usadas por la tarea SD
	display_message_t message;										//Para enviar mensajes al display
	char user_key_retrieved[SEQUENCE_LENGTH + 1];					//Para recibir los digitos desde el keypad

	//Control inicial. Verifico existencia de archivos importantes
	if(!initialize_sd_and_verify_files(&message)){
		//No se pudieron realizar las inicializaciones
		xQueueSend(display_queue, &message, portMAX_DELAY);			//Envio pantalla de error al display
		while(1);													//Loop permanente. Resetear y solucionar el problema.
	}

	while(1){
		//Recibo clave desde tarea keypad
		receive_user_key(user_key_retrieved);

		//Verifico si se ingreso clave para creacion de template
		if(is_template_creation(user_key_retrieved)){
			handle_template_creation(&message);
			continue;
		}

		//Realizo verificacion de usuario normal
		handle_user_verification(user_key_retrieved, &message);
	}
}

#endif


