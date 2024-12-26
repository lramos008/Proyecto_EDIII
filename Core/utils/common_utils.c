#include "utils.h"
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
