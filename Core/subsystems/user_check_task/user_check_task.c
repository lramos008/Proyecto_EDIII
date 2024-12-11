#include "main.h"
#include "cmsis_os.h"
#include "sd_functions.h"
#include "display_functions.h"
#include "dsp_functions.h"
#include <stdio.h>
#include <string.h>
/*================[Private defines]========================*/
#define SEQUENCE_LENGTH 6
#define USER_STR_SIZE 50
#define CODE_VERSION 3
/*================[Private variables]======================*/
display_state_t current_screen;
/*================[Extern variables]=======================*/
extern QueueHandle_t sequence_queue;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t conv_cplt_flag;

/*================[Private functions]======================*/
static uint32_t receive_sequence(void){
	uint32_t sequence;
	xQueueReceive(sequence_queue, &sequence, portMAX_DELAY);
	return sequence;
}

/*================[Public task]==========================*/
#if CODE_VERSION == 1
void user_check_task(void *pvParameters){
	uint32_t sequence;
	char *username;
	char *entry;
	char *rtc_reading;
	mount_sd("/");
	/*Verifico la existencia de base de datos*/
	if(check_if_file_exists("base_de_datos.txt") != FR_OK ){
		send_uart("No se encontro base de datos. Carguela.\n");
		while(1);
	}
	/*Verifico la existencia de un registro de accesos. De no existir, se crea*/
	if(check_if_file_exists("registro.txt") != FR_OK ){
		send_uart("Creando registro...\n");
		create_file("registro.txt", "Date User State\n");
		send_uart("Registro creado.\n");
	}
	unmount_sd("/");
	while(1){
		sequence = receive_sequence();
		mount_sd("/");
		username = pvPortMalloc(USER_STR_SIZE * sizeof(char));
		if(look_for_user("base_de_datos.txt", username, sequence) != FR_OK){
			entry = pvPortMalloc(50 * sizeof(char));
			rtc_reading = pvPortMalloc(20 * sizeof(char));
			get_time_from_rtc(rtc_reading);
			snprintf(entry, 50, "%s Usuario desconocido Denegado\n", rtc_reading);
			write_entry("registro.txt", entry);
			vPortFree(entry);
			vPortFree(rtc_reading);
		}
		else{
			uint16_t *buffer = pvPortMalloc(BUFFER_SIZE * sizeof(uint16_t));
			/*Capturo voz*/
			capture_voice(buffer, BUFFER_SIZE);					//Este tamaño de buffer equivale a 1.2 segundos aprox
			/*Guardo la voz capturada en la SD para procesarla por partes*/
			save_buffer_on_sd("current_voice.txt", buffer, BUFFER_SIZE);
			/*Libero la memoria del buffer para seguir realizando operaciones*/
			vPortFree(buffer);
			buffer = NULL;
			/*Leo bloque por bloque y voy procesando*/
			uint16_t *frame_buffer = pvPortMalloc(FRAME_SIZE * sizeof(uint16_t));
			float32_t *output = pvPortMalloc((FRAME_SIZE / 2) * sizeof(float32_t));
			float32_t *template = pvPortMalloc((FRAME_SIZE / 2) * sizeof(float32_t));
			char *path = pvPortMalloc(50 * sizeof(char));
			snprintf(path, 50, "%s.txt", username);
			FSIZE_t last_pos_1 = 0;
			FSIZE_t last_pos_2 = 0;
			for(uint32_t i = 0; i < (BUFFER_SIZE / FRAME_SIZE); i++){
				last_pos_1 = read_u16_buffer_from_sd("current_voice.txt", frame_buffer, FRAME_SIZE, last_pos_1);
				process_signal(frame_buffer, output, FRAME_SIZE);
				last_pos_2 = read_f_buffer_from_sd(path, template, FRAME_SIZE / 2, last_pos_2);
				if(compare_features(output, template, FRAME_SIZE / 2) == 1){
					entry = pvPortMalloc(50 * sizeof(char));
					rtc_reading = pvPortMalloc(20 * sizeof(char));
					get_time_from_rtc(rtc_reading);
					snprintf(entry, 50, "%s %s Concedido\n", rtc_reading, username);
					write_entry("registro.txt", entry);
					vPortFree(entry);
					vPortFree(rtc_reading);
					vPortFree(path);
					//Acceso concedido
				}
				else{
					//Acceso denegado
				}
			}
			vPortFree(frame_buffer);
			vPortFree(output);
			vPortFree(template);
		}
		vPortFree(username);
		unmount_sd("/");
	}
}

#elif CODE_VERSION == 2
/*Usar esta version para crear template con script de python*/
void user_check_task(void *pvParameters){
	volatile uint16_t *voice_buffer = (volatile uint16_t *) pvPortMalloc(BUFFER_SIZE * sizeof(uint16_t));
	for(uint8_t i = 0; i < 5; i++){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		vTaskDelay(5000 / portTICK_RATE_MS);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		vTaskDelay(100 / portTICK_RATE_MS);
		HAL_TIM_Base_Start_IT(&htim3);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)voice_buffer, BUFFER_SIZE);
		while(!conv_cplt_flag);
		conv_cplt_flag = 0;
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		send_buffer_via_uart(voice_buffer, BUFFER_SIZE);
	}
	vPortFree(voice_buffer);
	while(1){

	}
}

#elif CODE_VERSION == 3
/*Comprobacion de voz simple*/
void user_check_task(void *pvParameters){
	uint16_t buffer[5] = {1, 2, 3, 4, 5};

	mount_sd("/");
	save_buffer_on_sd("prueba.txt", buffer, 5);
	unmount_sd("/");
	while(1){

	}
}
#endif


