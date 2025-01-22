#include "access_control.h"
#include "file_handling.h"
#include "voice_recognition.h"
#include "sd_functions.h"
/*================[Private defines]========================*/
#define CODE_VERSION 2
#define TEMPLATE_P "template_file.bin"
#define TEST_P "test_file_ok.bin"
#define TEST_FALSE_P "test_file_false.bin"
/*================[Public task]==========================*/
#if CODE_VERSION == 1
void sd_task(void *pvParameters){
	//Variables usadas por la tarea SD
	display_message_t message;										//Para enviar mensajes al display
	char user_key_retrieved[SEQUENCE_LENGTH + 1];					//Para recibir los digitos desde el keypad
	bool res;

	//Montar tarjeta SD aca
	res = try_mount("");
	if(!res){
		send_error(DISPLAY_MOUNTING_ERROR);
		while(1);
	}

	//Control inicial. Verifico existencia de archivos importantes
	if(!initialize_sd_and_verify_files(&message)){
		//No se pudieron realizar las inicializaciones
		res = try_unmount("");										//Desmonto tarjeta SD
		send_error(message);
		while(1);													//Loop permanente. Resetear y solucionar el problema.
	}

	//Desmonto tarjeta SD
	res = try_unmount("");
	if(!res){
		send_error(DISPLAY_MOUNTING_ERROR);
		while(1);
	}

	while(1){
		//Recibo clave desde tarea keypad
		receive_user_key(user_key_retrieved);

		//Monto tarjeta SD
		res = try_mount("");
		if(!res){
			send_error(DISPLAY_MOUNTING_ERROR);
			while(1);
		}

		//Verifico si se ingreso clave para creacion de template
		if(is_template_creation(user_key_retrieved)){
			//Creo template
			handle_template_creation();

			//Desmonto tarjeta SD
			res = try_unmount("");
			if(!res){
				send_error(DISPLAY_MOUNTING_ERROR);
				while(1);
			}
			continue;
		}

		//Realizo verificacion de usuario normal
		handle_user_verification(user_key_retrieved);

		//Desmonto tarjeta SD
		res = try_unmount("");
		if(!res){
			send_error(DISPLAY_MOUNTING_ERROR);
			while(1);
		}
	}
}

#elif CODE_VERSION == 2
void sd_task(void *pvParameters){
	display_message_t message;
	bool res;
	bool is_recognized;
	//Monto tarjeta SD
	res = try_mount("");
	if(!res){
		message = DISPLAY_MOUNTING_ERROR;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}

	//Comparo features
	is_recognized = compare_features(TEMPLATE_P, TEST_P);

	//Desmonto tarjeta SD
	res = try_unmount("");
	if(!res){
		message = DISPLAY_MOUNTING_ERROR;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}

	while(1){

	}
}
#endif


