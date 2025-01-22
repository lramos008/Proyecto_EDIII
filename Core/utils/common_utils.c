#include "common_utils.h"
//Funciones utiles
void clear_buffer(char *buffer, size_t length) {
    if (buffer == NULL) {
        return; // Verifica que el puntero no sea nulo
    }
    for (size_t i = 0; i < length; i++) {
        buffer[i] = '\0'; // Llena el buffer con caracteres nulos
    }
}

void clear_char(char *buffer, char character){
	char *ptr = buffer;
	while(*ptr != character){
		if(*ptr == 0){
			break;
		}
		ptr++;
	}
	*ptr = 0;
	return;
}


void send_message(display_message_t message, bool is_blocking){
	xQueueSend(display_queue, &message, portMAX_DELAY);					//Envio mensaje de error al display

	//Compruebo si es bloqueante el mensaje
	if(is_blocking){
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);					//Esperar hasta que el display termine de mostrar el mensaje
	}
	return;
}

void send_error(display_message_t error_message){
	send_message(error_message, BLOCKING);
	return;
}
