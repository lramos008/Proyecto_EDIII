#include "common_utils.h"
#include "file_handling.h"
#include "voice_recognition.h"
#include "template_creation.h"
#include "sd_functions.h"

#define TEMPLATE_CREATION_SEQUENCE "000000"

void receive_user_key(char *user_key){
	for(uint8_t i = 0; i < SEQUENCE_LENGTH; i++){
		xQueueReceive(sequence_queue, &user_key[i], portMAX_DELAY);
	}
	return;
}

bool is_template_creation(char *user_key){
	return strcmp(user_key, TEMPLATE_CREATION_SEQUENCE) == 0;					//Si coinciden las secuencias, devuelve true
}

void handle_template_creation(void){
	//Declaracion de variables
	display_message_t message;

	//Genero template
	message = generate_template() ? DISPLAY_TEMPLATE_SAVED : DISPLAY_TEMPLATE_NOT_CREATED;

	//Envio mensaje al display
	xQueueSend(display_queue, &message, portMAX_DELAY);
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);								//Espero a que el display termine de mostrar su mensaje
	xSemaphoreGive(keypad_sd_sync);												//Cedo semaforo para que el keypad pueda volver a ejecutarse
}

void handle_user_verification(char *user_key){
	char user_name[USER_STR_SIZE];
	char template_path[TEMPLATE_STR_SIZE];
	display_message_t message;

	//Verifico existencia de usuario en base de datos
	if(!process_user_key(user_key, user_name)){
		//Usuario no encontrado
		message = DISPLAY_USER_NOT_FOUND;
		send_error(message);
		xSemaphoreGive(keypad_sd_sync);											//Devuelvo el control a la tarea keypad
		return;
	}

	//Indicar que el usuario fue encontrado
	message = DISPLAY_USER_FOUND;
	xQueueSend(display_queue, &message, portMAX_DELAY);

	//Verifico que exista template asociado al usuario
	snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
	if(check_if_file_exists(template_path) != FR_OK){
		//Template no existe
		message = DISPLAY_TEMPLATE_NOT_FOUND;
		send_error(message);
		xSemaphoreGive(keypad_sd_sync);
		return;
	}

	//El template existe, procedo al reconocimiento de voz
	message = recognize_user_voice(template_path, user_name) ? DISPLAY_ACCESS_GRANTED : DISPLAY_ACCESS_DENIED;

	//Envio mensaje al display
	xQueueSend(display_queue, &message, portMAX_DELAY);
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);
	xSemaphoreGive(keypad_sd_sync);
	return;
}
