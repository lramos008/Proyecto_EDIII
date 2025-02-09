#include "common_utils.h"
#include "keypad_functions.h"
/*=======================[Private Defines]==========================*/
#define BORRAR_DIGITO '#'
#define ENTER '*'
#define KEYPAD_TIMEOUT 5000						//en ms
/*=====================[Private data types]=========================*/
typedef enum{
	WAITING_FOR_DIGIT_1,
	WAITING_FOR_DIGIT_2,
	WAITING_FOR_DIGIT_3,
	WAITING_FOR_DIGIT_4,
	WAITING_FOR_DIGIT_5,
	WAITING_FOR_DIGIT_6,
	WAITING_FOR_CONFIRMATION,
	SEND_SEQUENCE_TO_SD,
	INCOMPLETE_SEQUENCE,
	TIMEOUT
}keypad_state_t;
/*================[Private Structures]=======================*/
typedef struct{
	keypad_state_t current_state;
	uint8_t current_index;
	TickType_t start_time;
}keypad_context_t;

/*================[Public functions]=========================*/
display_message_t handle_keypad_input(char input, char *buffer, bool *send_flag){
	static keypad_context_t context = { .current_state = WAITING_FOR_DIGIT_1,
										.current_index = 0,
										.start_time = 0,					};
	display_message_t current_message = DISPLAY_IDLE;
	switch(context.current_state){
	case WAITING_FOR_DIGIT_1:
		if(input != 0 && input != BORRAR_DIGITO && input != ENTER){
			buffer[0] = input;													//Guardo digito en el buffer
			context.current_index = 1;
			context.start_time = xTaskGetTickCount();							//Reinicio el timeout
			context.current_state = WAITING_FOR_DIGIT_2;						//Avanzo al siguiente estado
			current_message = DISPLAY_ENTER_DIGIT;
		}
		else{
			current_message = DISPLAY_IDLE;
		}
		break;
	case WAITING_FOR_DIGIT_2:
	case WAITING_FOR_DIGIT_3:
	case WAITING_FOR_DIGIT_4:
	case WAITING_FOR_DIGIT_5:
	case WAITING_FOR_DIGIT_6:
	case WAITING_FOR_CONFIRMATION:
		if((xTaskGetTickCount() - context.start_time) < pdMS_TO_TICKS(KEYPAD_TIMEOUT)){
			if(input != 0){
				if(input == BORRAR_DIGITO){
					if(context.current_index > 0){
						context.current_index--;
						buffer[context.current_index] = '\0';					//Borro el contenido previo
						context.start_time = xTaskGetTickCount();				//Reinicio el timeout
						current_message = DISPLAY_ERASE_DIGIT;
						context.current_state = (context.current_state > WAITING_FOR_DIGIT_1) ? context.current_state - 1 : WAITING_FOR_DIGIT_1;
						if(context.current_state == WAITING_FOR_DIGIT_1){
							current_message = DISPLAY_SCREEN_WELCOME;
						}
					}
				}
				else if(input == ENTER){
					if(context.current_state == WAITING_FOR_CONFIRMATION){
						context.current_state = SEND_SEQUENCE_TO_SD;
						//En este caso no hace falta devolver la pantalla
					}
					else{
						context.current_state = INCOMPLETE_SEQUENCE;
						current_message = DISPLAY_INCOMPLETE_SEQUENCE_EVENT;
					}
				}
				else{
					if(context.current_index < SEQUENCE_LENGTH){
						buffer[context.current_index++] = input;				//Agrego el digito al buffer
						buffer[context.current_index] = '\0';					//Aseguro que el buffer siempre termine en caracter nulo
						context.start_time = xTaskGetTickCount();				//Reinicio el timeout
						current_message = DISPLAY_ENTER_DIGIT;
						if(context.current_state != WAITING_FOR_CONFIRMATION){
							context.current_state++;
						}
					}
				}
			}
			else{
				current_message = DISPLAY_IDLE;
			}
		}
		else{
			context.current_state = TIMEOUT;
			current_message = DISPLAY_TIMEOUT_EVENT;
		}
		break;
	case SEND_SEQUENCE_TO_SD:
		*send_flag = 1;															//Habilito flag para enviar el string
		context.current_state = WAITING_FOR_DIGIT_1;
		context.current_index = 0;
		current_message = DISPLAY_SCREEN_WELCOME;								//Luego del procesamiento se vuelve al estado inicial
		break;
	case INCOMPLETE_SEQUENCE:
	case TIMEOUT:
	default:
		clear_buffer(buffer, SEQUENCE_LENGTH + 1);
		context.current_state = WAITING_FOR_DIGIT_1;							//Vuelvo al estado inicial
		context.current_index = 0;
		current_message = DISPLAY_SCREEN_WELCOME;
		break;
	}
	return current_message;
}



