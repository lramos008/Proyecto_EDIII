#include "common_utils.h"

#define TEMPLATE_CREATION_SEQUENCE "000000"

void receive_user_key(char *user_key){
	for(uint8_t i = 0; i < SEQUENCE_LENGTH; i++){
		xQueueReceive(sequence_queue, &user_key[i], portMAX_DELAY);
	}
	return;
}

bool is_template_creation(char *user_key){
	return strcmp(user_key, TEMPLATE_CREATION_SEQUENCE) == 0;
}

void handle_template_creation(display_message_t *message){
	if(generate_template()){
		*message = DISPLAY_TEMPLATE_SAVED;
	}
	else{
		*message = DISPLAY_TEMPLATE_NOT_CREATED;
	}

	//Envio mensaje al display
	xQueueSend(display_queue, message, portMAX_DELAY);

	//Sincronizo con las otras tareas
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);
	xSemaphoreGive(keypad_sd_sync);
}

void handle_user_verification(char *user_key, display_message_t *message){
	char user_name[USER_STR_SIZE];
	char template_path[TEMPLATE_STR_SIZE];

	//Verifico existencia de usuario en base de datos
	if(!process_user_key(user_key, user_name, message)){
		//Usuario no encontrado
		xQueueSend(display_queue, message, portMAX_DELAY);
		xSemaphoreGive(keypad_sd_sync);
		return;
	}

	//Indicar que el usuario fue encontrado
	*message = DISPLAY_USER_FOUND;
	xQueueSend(display_queue, message, portMAX_DELAY);

	//Verifico que exista template asociado al usuario
	snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
	if(check_if_file_exists(template_path) != FR_OK){
		//Template no existe
		*message = DISPLAY_TEMPLATE_NOT_FOUND;
		xQueueSend(display_queue, message, portMAX_DELAY);
		xSemaphoreGive(keypad_sd_sync);
		return;
	}

	//El template existe, procedo al reconocimiento de voz
	if(recognize_user_voice(template_path, user_name, message)){
		message = DISPLAY_ACCESS_GRANTED;
	}
	else{
		message = DISPLAY_ACCESS_DENIED;
	}

	//Envio mensaje al display
	xQueueSend(display_queue, message, portMAX_DELAY);
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);
	xSemaphoreGive(keypad_sd_sync);
	return;

}
