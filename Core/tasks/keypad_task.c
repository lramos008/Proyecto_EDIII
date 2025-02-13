#include <digit_handle_logic.h>
#include "keypad_functions.h"
/*================[Public Task]=====================*/
void keypad_task(void *pvParameters){
	char input;
	char kp_buffer[SEQUENCE_LENGTH + 1] = {0};										//Son 7 elementos para incluir el caracter '\0'
	bool send_flag = false;
	display_message_t message;
	while(1){
		input = read_keypad();
		message = handle_keypad_input(input, kp_buffer, &send_flag);				//Manejo la logica de recepcion.
		if(send_flag){
			//Envio uno por uno los digitos obtenidos con el keypad a la tarea SD
			for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
				xQueueSend(sequence_queue, &kp_buffer[i], portMAX_DELAY);
			}
			xSemaphoreTake(keypad_sd_sync, portMAX_DELAY);							//Espero que la tarea SD devuelva el semaforo

			//Reinicio el flag de envio
			send_flag = false;

			//Limpio el vector char que guarda la secuencia
			clear_buffer(kp_buffer, SEQUENCE_LENGTH);
		}

		//Envio el mensaje a la tarea display si no hay pantalla idle
		if(message != DISPLAY_IDLE){
			xQueueSend(display_queue, &message, portMAX_DELAY);
		}
	}
}


