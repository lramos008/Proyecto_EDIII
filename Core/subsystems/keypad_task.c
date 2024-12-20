#include "keypad_functions.h"
#include "input_sequence_handle.h"
#include "display_functions.h"

char kp_buffer[SEQUENCE_LENGTH + 1] = {0};
/*================[Public Task]=====================*/
void keypad_task(void *pvParameters){
	char input;
	uint8_t send_flag = 0;
	indicatorMessage display_message;
	//Doy el semaforo para que lo pueda tomar la tarea SD cuando le toque
	xSemaphoreGive(keypad_sd_sync);
	while(1){
		input = read_keypad();
		display_message = handle_keypad_input(input, kp_buffer, &send_flag);
		if(send_flag){
			//Envio uno por uno los datos del buffer
			for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
				xQueueSend(sequence_queue, &kp_buffer[i], portMAX_DELAY);
			}

			//Bloqueo la tarea keypad hasta que la tarea SD me devuelva el semaforo
			xSemaphoreTake(keypad_sd_sync, portMAX_DELAY);

			//Reinicio el flag de envio
			send_flag = 0;
		}
		//Envio el mensaje a la tarea display si no hay pantalla idle
		if(display_message != PANTALLA_IDLE){
			xQueueSend(display_queue, &display_message, portMAX_DELAY);
		}
	}
}


