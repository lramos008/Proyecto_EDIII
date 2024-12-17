#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "utils.h"

#define LINE_HEIGHT 18  // Altura de la fuente (ajustar según la fuente usada)
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
/*================[Public functions]=====================*/
void display_init(void){
	/*Wrapper de la función que inicializa el display*/
	ssd1306_Init();
	return;
}

void display_text(const char *text,  uint8_t font_size) {
    uint8_t x = 0;  			// Posición inicial en X
    uint8_t y = 0;  			// Posición inicial en Y
    uint8_t char_width = 0;   	// Ancho del carácter actual
    uint8_t line_height = 0;  	// Altura de la línea actual
    // Configurar la fuente según el tamaño
    switch (font_size) {
        case 1: 									// Tamaño pequeño
            char_width = 7;
            line_height = 10;
            break;
        case 2: 									// Tamaño mediano
            char_width = 11;
            line_height = 18;
            break;
        case 3: 									// Tamaño grande
            char_width = 16;
            line_height = 24;
            break;
        default: 									// Tamaño por defecto
            char_width = 7;
            line_height = 10;
            break;
    }

    ssd1306_Fill(Black);  // Limpiar la pantalla

    while (*text) {
        // Verificar si es un salto de línea manual
        if (*text == '\n') {
            y += line_height;  											// Avanzo a la siguiente linea
            x = 0;             											// Reinicio x
        } else {
            // Escribir el carácter actual
            ssd1306_SetCursor(x, y);
            if(font_size == 1){
            	ssd1306_WriteChar(*text, Font_7x10, White);
            }
            else if(font_size == 2){
            	ssd1306_WriteChar(*text, Font_11x18, White);
            }
            else if(font_size == 3){
            	ssd1306_WriteChar(*text, Font_16x24, White);
            }
            else{
            	ssd1306_WriteChar(*text, Font_7x10, White);
            }

            // Avanzar la posición X
            x += char_width;  // Avanzar por el ancho de la fuente
            if (x + char_width > DISPLAY_WIDTH) {  							// Si se pasa del ancho de pantalla
                x = 0;                      								// Reinicio x
                y += line_height;           								// Avanzo a la siguiente linea
            }
        }

        // Avanzar al siguiente carácter
        text++;

        // Verificar si alcanzamos el final de la pantalla
        if (y + line_height > DISPLAY_HEIGHT) {
            break;  // No hay más espacio para mostrar texto
        }
    }

    ssd1306_UpdateScreen();  // Actualizar pantalla
}

void display_start_msg(void){
	/*Muestra mensaje de insertar clave*/
	uint8_t x = 5;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Ingrese", Font_16x26, White);
	x += 15;
	y += 28;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("clave", Font_16x26, White);
	ssd1306_UpdateScreen();
	return;
}

void display_sequence_entry_msg(uint8_t input_cont){
	/*Maneja el ingreso de digitos en el display*/
	char message[MAX_DIGITS + 1] = {0};
	uint8_t x = 10;														//x e y definen coordenadas en pantalla
	uint8_t y = 26;
	//Chequeo que counter no supere el numero maximo de digitos
	if(input_cont <= MAX_DIGITS){
		//Armo el string a mostrar en el display
		for(uint8_t i = 0; i < input_cont; i++){
			message[i] = '*';
		}
		//Muestro el string
		ssd1306_Fill(Black);
		ssd1306_SetCursor(x, y);
		ssd1306_WriteString(message, Font_16x26, White);
		ssd1306_UpdateScreen();
	}
	return;
}


void display_timeout_msg(void){
	display_text("\n Timeout!!!", 2);
	return;
}

void display_incomplete_entry_msg(void){
	display_text("  Ingrese\n secuencia\n  valida", 2);
	return;
}

void display_access_granted_msg(void){
	display_text("  Acceso\n concedido", 2);
	return;
}

void display_user_found_msg(void){
	display_text("  Usuario\n  hallado", 2);
	return;
}

void display_user_not_found_msg(void){
	display_text("  Usuario\n    no\n  existe", 2);
	return;
}

void display_start_voice_recognition_msg(void){
	display_text("  Comienza\n  captura\n  de voz", 2);
	return;
}

void countdown_msg(void){
	/*Muestra mensaje de cuenta regresiva en pantalla*/
	uint8_t x = 50;
	uint8_t y = 26;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("3", Font_16x26, White);
	ssd1306_UpdateScreen();
	vTaskDelay(1000/portTICK_RATE_MS);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("2", Font_16x26, White);
	ssd1306_UpdateScreen();
	vTaskDelay(1000/portTICK_RATE_MS);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("1", Font_16x26, White);
	ssd1306_UpdateScreen();
	vTaskDelay(1000/portTICK_RATE_MS);
}

void display_capturing_voice_msg(void){
	display_text("  Captura\n    ...", 2);
	return;
}

void display_recognized_voice_msg(void){
	display_text("    Voz\n reconocida", 2);
	return;
}


void display_not_recognized_voice_msg(void){
	display_text("   Voz no\n reconocida", 2);
	return;
}

void display_processing_data_msg(void){
	display_text("  Aguarde\n    ...", 2);
	return;
}

void display_missing_database_msg(void){
	display_text("  Database\n  faltante", 2);
	return;
}

void display_missing_template_msg(void){
	display_text("  Template\n  faltante", 2);
	return;
}

void display_template_saved_msg(void){
	display_text("  Template\n  guardado", 2);
	return;
}




