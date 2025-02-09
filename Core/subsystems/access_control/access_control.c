#include "common_utils.h"
#include "file_handling.h"
#include "voice_recognition.h"
#include "template_creation.h"
#include "sd_functions.h"
#include "user_and_entry.h"

#define TEMPLATE_CREATION_SEQUENCE "000000"
#define TEMPLATE_FOLDER "templates"

/**
 * @brief Recibe secuencia ingresada por el usuario desde la tarea keypad.
 *
 * @param user_key Puntero char al vector donde se guardara la secuencia.
 * @return None
 */
void receive_user_key(char *user_key){
	for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
		xQueueReceive(sequence_queue, &user_key[i], portMAX_DELAY);
	}
	return;
}


/**
 * @brief Chequea si se ingreso secuencia para creación de template.
 *
 * @param user_key Puntero char que apunta al inicio de la secuencia.
 * @return true si la secuencia coincide con la secuencia de creacion de template, false en caso contrario.
 */
bool is_template_creation(char *user_key){
	return strcmp(user_key, TEMPLATE_CREATION_SEQUENCE) == 0;
}


/**
 * @brief Maneja la generacion de voces para la creacion del template.
 *
 * Esta función captura señales de voz para crear el template y las
 * envia via uart hacia la PC. Alli se creara el template de voz.
 *
 * @param None
 * @return None
 */
void handle_template_creation(void){
	//Declaracion de variables
	display_message_t message;

	//Genero template
	message = generate_template() ? DISPLAY_TEMPLATE_SAVED : DISPLAY_TEMPLATE_NOT_CREATED;

	//Envio mensaje al display
	send_message(message, BLOCKING);

	//Cedo semaforo para que el keypad pueda continuar ejecutandose
	xSemaphoreGive(keypad_sd_sync);
	return;
}

/**
 * @brief Realiza comprobaciones referentes al usuario para decidir si se da acceso o no a la persona.
 *
 * Esta función verifica la existencia del usuario en la base de datos. De hallarse, verifica la existencia
 * de un template asociado a ese usuario. De existir este template, se realizara captura de voz, extracción
 * de features de esa voz y luego la posterior comparacion con el template para decidir si  la voz es
 * reconocida o no.
 *
 * @param user_key Puntero char que apunta al inicio de la secuencia.
 * @return None
 */

void handle_user_verification(char *user_key){
	char user_name[USER_STR_SIZE] = {0};
	char template_path[TEMPLATE_STR_SIZE] = {0};
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
	send_message(DISPLAY_USER_FOUND, NON_BLOCKING);

	//Verifico que exista template asociado al usuario
	//snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
	snprintf(template_path, TEMPLATE_STR_SIZE, "/%s/%s.bin", TEMPLATE_FOLDER, user_name);
	if(!check_if_file_exists(template_path)){
		//Template no encontrado
		send_error(DISPLAY_TEMPLATE_NOT_FOUND);
		xSemaphoreGive(keypad_sd_sync);
		return;
	}
	//El template existe, procedo al reconocimiento de voz
	message = recognize_user_voice(template_path, user_name) ? DISPLAY_ACCESS_GRANTED : DISPLAY_ACCESS_DENIED;
	//Envio mensaje al display
	send_message(message, BLOCKING);

	//Cedo semaforo para que el keypad pueda continuar ejecutandose
	xSemaphoreGive(keypad_sd_sync);
	return;
}
