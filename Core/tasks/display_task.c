#include "display_functions.h"
#include "common_utils.h"
#define DISPLAY_FUNCTION 1										//0 realiza prueba de display completa
																//1 provee la version funcional del codigo

/*================[Public tasks]=========================*/
#if DISPLAY_FUNCTION == 0
//Version para verificar el funcionamiento del display
void display_task(void *pvParameters){
	display_init();
	while(1){
		display_start_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		for(uint8_t i = 0; i < MAX_DIGITS; i++){
			display_sequence_entry_msg(i);
			vTaskDelay(200 / portTICK_RATE_MS);
		}
		for(uint8_t i = 6; i > 0; i--){
			display_sequence_entry_msg(i);
			vTaskDelay(200 / portTICK_RATE_MS);
		}
		display_access_granted_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		display_timeout_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		display_incomplete_entry_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		display_user_not_found_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		display_start_voice_recognition_msg();
		vTaskDelay(1000 / portTICK_RATE_MS);
		countdown_msg();
		display_capturing_voice_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_not_recognized_voice_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_recognized_voice_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_processing_data_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_missing_database_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_missing_template_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
		display_template_saved_msg();
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

#elif DISPLAY_FUNCTION == 1
//Version funcional que utiliza el control de acceso
void display_task(void *pvParameters){
	display_message_t message;
	uint8_t counter = 0;
	display_init();						//Inicializo el display
	display_start_msg();				//Muestro mensaje inicial
	while(1){
		xQueueReceive(display_queue, &message, portMAX_DELAY);
		switch(message){
		case DISPLAY_SCREEN_WELCOME:
			display_start_msg();
			break;
		case DISPLAY_ENTER_DIGIT:
			if(counter < MAX_DIGITS){
				counter++;
				display_sequence_entry_msg(counter);
			}
			break;
		case DISPLAY_ERASE_DIGIT:
			if(counter > 0){
				counter--;
				display_sequence_entry_msg(counter);
			}
			break;
		case DISPLAY_USER_FOUND:
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);			//Se utiliza para sincronizar las tareas sd y display
			display_user_found_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			xSemaphoreGive(sd_display_sync);
			break;
		case DISPLAY_USER_NOT_FOUND:
			display_user_not_found_msg();
			counter = 0;							//Reinicio el contador
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_START_SPEECH_REC:
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);
			display_start_voice_recognition_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			countdown_msg();
			display_capturing_voice_msg();
			xSemaphoreGive(sd_display_sync);
			break;
		case DISPLAY_VOICE_RECOGNIZED:
			display_recognized_voice_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_VOICE_NOT_RECOGNIZED:
			display_not_recognized_voice_msg();
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);
			vTaskDelay(2000 / portTICK_RATE_MS);
			xSemaphoreGive(sd_display_sync);
			break;
		case DISPLAY_PROCESSING_DATA:
			display_processing_data_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_ACCESS_GRANTED:
			display_access_granted_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_TIMEOUT_EVENT:
			display_timeout_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_INCOMPLETE_SEQUENCE_EVENT:
			display_incomplete_entry_msg();
			counter = 0;
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_DATABASE_NOT_FOUND:
			display_missing_database_msg();
			break;
		case DISPLAY_TEMPLATE_NOT_FOUND:
			display_missing_template_msg();
			vTaskDelay(2000 / portTICK_RATE_MS);
			break;
		case DISPLAY_TEMPLATE_SAVED:
			display_template_saved_msg();
			break;
		default:
		}
	}
}
#endif

