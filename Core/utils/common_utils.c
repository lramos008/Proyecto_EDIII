#include "common_utils.h"

extern UART_HandleTypeDef huart2;

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

void generate_filename(char *filename, uint32_t x){
	snprintf(filename, "voice_%d.bin", x);
	return;
}

bool send_uart(uint8_t *buffer, uint16_t size){
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, buffer, size, HAL_MAX_DELAY);
	return (status == HAL_OK) ? true : false;
}
