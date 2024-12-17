#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "utils.h"

//#define LINE_HEIGHT 18  // Altura de la fuente (ajustar según la fuente usada)
//#define DISPLAY_WIDTH 128
//#define DISPLAY_HEIGHT 64
/*================[Public functions]=====================*/
void display_init(void){
	/*Wrapper de la función que inicializa el display*/
	ssd1306_Init();
	return;
}

//void display_text(const char *text) {
//    uint8_t x = 0;  // Posición inicial en X
//    uint8_t y = 0;  // Posición inicial en Y
//
//    ssd1306_Fill(Black);  // Limpiar la pantalla
//
//    while (*text) {
//        // Verificar si es un salto de línea manual
//        if (*text == '\n') {
//            y += LINE_HEIGHT;  // Avanzar a la siguiente línea
//            x = 0;             // Reiniciar X
//        } else {
//            // Escribir el carácter actual
//            ssd1306_SetCursor(x, y);
//            ssd1306_WriteChar(*text, Font_11x18, White);
//
//            // Avanzar la posición X
//            x += 11;  // Avanzar por el ancho de la fuente (ajustar según la fuente)
//            if (x + 11 > DISPLAY_WIDTH) {  // Si se pasa del ancho de pantalla
//                x = 0;                      // Reiniciar X
//                y += LINE_HEIGHT;           // Avanzar a la siguiente línea
//            }
//        }
//
//        // Avanzar al siguiente carácter
//        text++;
//
//        // Verificar si alcanzamos el final de la pantalla
//        if (y + LINE_HEIGHT > DISPLAY_HEIGHT) {
//            break;  // No hay más espacio para mostrar texto
//        }
//    }
//
//    ssd1306_UpdateScreen();  // Actualizar pantalla
//}

void display_start_msg(void){
	/*Muestra mensaje de insertar clave*/
	uint8_t x = 5;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Ingrese", Font_16x26, White);
	x += 15;
	y += 26;
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
	/*Muestra mensaje de timeout en display*/
	uint8_t x = 5;
	uint8_t y = 18;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Timeout!!!", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_incomplete_entry_msg(void){
	/*Muestra mensaje de secuencia incompleta en display*/
	uint8_t x = 20;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Ingrese", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("secuencia", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("valida", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_access_granted_msg(void){
	/*Muestra mensaje de acceso concedido*/
	uint8_t x = 5;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Acceso", Font_11x18, White);
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("concedido", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_user_found_msg(void){
	/*Muestra mensaje de usuario encontrado*/
	uint8_t x = 20;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Usuario", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("hallado", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_user_not_found_msg(void){
	/*Muestra mensaje de usuario no encontrado*/
	uint8_t x = 20;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Usuario", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("no", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("existente", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_start_voice_recognition_msg(void){
	/*Muestra mensaje que indica el inicio de reconocimiento de voz*/
	uint8_t x = 20;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Comienza", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("captura ", Font_11x18, White);
	x = 20;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("de voz", Font_11x18, White);
	ssd1306_UpdateScreen();
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
	/*Muestra mensaje que indica que se esta capturando voz*/
	uint8_t x = 10;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Capturando", Font_11x18, White);
	x = 50;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("...", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_recognized_voice_msg(void){
	/*Muestra mensaje de voz reconocida*/
	uint8_t x = 40;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Voz", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("reconocida", Font_11x18, White);
	ssd1306_UpdateScreen();
}


void display_not_recognized_voice_msg(void){
	/*Muestra mensaje de voz no reconocida*/
	uint8_t x = 20;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Voz no", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("reconocida", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_processing_data_msg(void){
	/*Muestra mensaje de voz reconocida*/
	uint8_t x = 40;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Procesando", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("datos", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_missing_database_msg(void){
	/*Muestra mensaje de database faltante*/
	uint8_t x = 40;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Database", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("faltante", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_missing_template_msg(void){
	/*Muestra mensaje de template faltante*/
	uint8_t x = 40;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Template", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("faltante", Font_11x18, White);
	ssd1306_UpdateScreen();
}

void display_template_saved_msg(void){
	/*Muestra mensaje de template faltante*/
	uint8_t x = 40;
	uint8_t y = 0;
	ssd1306_Fill(Black);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Template", Font_11x18, White);
	x = 10;
	y += 18;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("guardado", Font_11x18, White);
	ssd1306_UpdateScreen();
}




