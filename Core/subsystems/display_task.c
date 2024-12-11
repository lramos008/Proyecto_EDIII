#include "display_functions.h"
#include "utils.h"
#define DISPLAY_FUNCTION 1										//0 realiza prueba de display completa
																//1 provee la version funcional del codigo

/*================[Public tasks]=========================*/
#if DISPLAY_FUNCTION == 0
//Version para verificar el funcionamiento del display
void display_task(void *pvParameters){
	display_init();
	while(1){
		display_start_msg();
		vTaskDelay(500 / portTICK_RATE_MS);
		for(uint8_t i = 0; i < MAX_DIGITS; i++){
			display_sequence_entry_msg(i);
			vTaskDelay(200 / portTICK_RATE_MS);
		}
		for(uint8_t i = 6; i > 0; i--){
			display_sequence_entry_msg(i);
			vTaskDelay(200 / portTICK_RATE_MS);
		}
		display_access_granted_msg();
		vTaskDelay(500 / portTICK_RATE_MS);
		display_timeout_msg();
		vTaskDelay(500 / portTICK_RATE_MS);
		display_incomplete_entry_msg();
		vTaskDelay(500 / portTICK_RATE_MS);
		display_user_not_found_msg();
		vTaskDelay(500 / portTICK_RATE_MS);
		display_start_voice_recognition_msg();
		vTaskDelay(2000 / portTICK_RATE_MS);
		countdown_msg();
		display_capturing_voice_msg();
		vTaskDelay(3000/portTICK_RATE_MS);
		display_not_recognized_voice_msg();
		vTaskDelay(2000/portTICK_RATE_MS);
		display_recognized_voice_msg();
		vTaskDelay(2000/portTICK_RATE_MS);
	}
}

#elif DISPLAY_FUNCTION == 1
//Version funcional que utiliza el control de acceso
void display_task(void *pvParameters){
	indicatorMessage display_message;
	uint8_t counter = 0;
	display_init();						//Inicializo el display
	display_start_msg();				//Muestro mensaje inicial
	while(1){
		xQueueReceive(display_queue, &display_message, portMAX_DELAY);
		switch(display_message){
		case PANTALLA_DE_INICIO:
			display_start_msg();
			break;
		case PANTALLA_INGRESO_KEYPAD:
			if(counter < MAX_DIGITS){
				counter++;
				display_sequence_entry_msg(counter);
			}
			break;
		case PANTALLA_BORRAR_KEYPAD:
			if(counter > 0){
				counter--;
				display_sequence_entry_msg(counter);
			}
			break;
		case PANTALLA_USUARIO_ENCONTRADO:
			display_user_found_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_USUARIO_NO_EXISTE:
			display_user_not_found_msg();
			counter = 0;							//Reinicio el contador
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_RECONOCIMIENTO_DE_VOZ:
			display_start_voice_recognition_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			countdown_msg();
			display_capturing_voice_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_VOZ_RECONOCIDA:
			display_recognized_voice_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_VOZ_NO_RECONOCIDA:
			display_not_recognized_voice_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_ACCESO_CONCEDIDO:
			display_access_granted_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_TIMEOUT:
			display_timeout_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case PANTALLA_SECUENCIA_INCOMPLETA:
			display_incomplete_entry_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		default:
		}
	}
}
#endif

