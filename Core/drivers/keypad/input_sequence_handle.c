#include "utils.h"
/*==================[Private Defines]========================*/
#define BORRAR_DIGITO '#'
#define ENTER '*'
#define KEYPAD_TIMEOUT 5000
/*================[Private Structures]=======================*/
typedef struct{
	keypad_state_t current_state;
	uint8_t current_index;
	TickType_t start_time;
	char buffer[SEQUENCE_LENGTH + 1];
}keypad_context_t;

void clear_buffer(char *buffer, size_t length) {
    if (buffer == NULL) {
        return; // Verifica que el puntero no sea nulo
    }
    for (size_t i = 0; i < length; i++) {
        buffer[i] = '\0'; // Llena el buffer con caracteres nulos
    }
}

static indicatorMessage handle_keypad_input(char input){
	static keypad_context_t context = { .current_state = ESPERANDO_DIGITO_1,
										.current_index = 0,
										.start_time = 0,
										.buffer = {0}						};
	indicatorMessage current_message = PANTALLA_IDLE;
	switch(context.current_state){
	case ESPERANDO_DIGITO_1:
		if(input != 0 && input != BORRAR_DIGITO && input != ENTER){
			context.buffer[0] = input;											//Guardo digito en el buffer
			context.current_index = 1;
			context.start_time = xTaskGetTickCount();							//Reinicio el timeout
			context.current_state = ESPERANDO_DIGITO_2;							//Avanzo al siguiente estado
			current_message = PANTALLA_INGRESO_KEYPAD;
		}
		else{
			current_message = PANTALLA_DE_INICIO;
		}
		break;
	case ESPERANDO_DIGITO_2:
	case ESPERANDO_DIGITO_3:
	case ESPERANDO_DIGITO_4:
	case ESPERANDO_DIGITO_5:
	case ESPERANDO_DIGITO_6:
	case ESPERANDO_CONFIRMACION:
		if((xTaskGetTickCount() - context.start_time) < pdMS_TO_TICKS(5000)){
			if(input != 0){
				if(input == BORRAR_DIGITO){
					if(context.current_index > 0){
						context.current_index--;
						context.buffer[context.current_index] = '\0';			//Borro el contenido previo
						context.start_time = xTaskGetTickCount();				//Reinicio el timeout
						current_message = PANTALLA_BORRAR_KEYPAD;
						context.current_state = (context.current_state > ESPERANDO_DIGITO_1) ? context.current_state - 1 : ESPERANDO_DIGITO_1;
					}
				}
				else if(input == ENTER){
					if(context.current_state == ESPERANDO_CONFIRMACION){
						context.current_state = BUSQUEDA_DE_USUARIO;
						//En este caso no hace falta devolver la pantalla
					}
					else{
						context->current_state = SECUENCIA_INCOMPLETA;
						current_message = PANTALLA_SECUENCIA_INCOMPLETA;
					}
				}
				else{
					if(context.current_index < SEQUENCE_LENGTH){
						context.buffer[context.current_index++] = input;		//Agrego el digito al buffer
						context.buffer[context.current_index] = '\0';			//Aseguro que el buffer siempre termine en caracter nulo
						context.start_time = xTaskGetTickCount();				//Reinicio el timeout
						current_message = PANTALLA_INGRESO_KEYPAD;
						if(context.current_state != ESPERANDO_CONFIRMACION){
							context.current_state++;
						}
					}
				}
			}
		}
		else{
			context.current_state = TIMEOUT;
			current_message = PANTALLA_TIMEOUT;
		}
		break;
	case BUSQUEDA_DE_USUARIO:
		//Enviar la secuencia a la tarea SD
		//Resetear la secuencia
		break;
	case SECUENCIA_INCOMPLETA:
	case TIMEOUT:
	default:
		clear_buffer(context.buffer, SEQUENCE_LENGTH + 1);
		context.current_state = ESPERANDO_DIGITO_1;								//Vuelvo al estado inicial
		context.current_index = 0;
		current_message = PANTALLA_DE_INICIO;
		break;
	}
	return current_message;
}
