#include "common_utils.h"
#include "sd_functions.h"

/*================[Private defines]======================*/
#define ADC_RESOLUTION 4096.0
#define VOLTAGE_REFERENCE 3.3
/*================[Extern variables]=====================*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern volatile bool conv_cplt_flag;

/*================[Public functions]=====================*/
static void capture_voice(uint16_t *buffer, uint32_t size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, size);					//Inicio captura de datos con ADC
	while(!conv_cplt_flag);												//Espero a que termine la captura
	conv_cplt_flag = false;
	return;
}

static void get_voltage(uint16_t *in_buffer, float *out_buffer, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out_buffer[i] = VOLTAGE_REFERENCE * ((float)in_buffer[i]) / ADC_RESOLUTION;
	}
}

//Debo cambiar este codigo
static bool store_voice(uint16_t *voice_buffer, uint32_t total_size, uint32_t block_size, char *filename){
	float *current_block;
	//Verifico que size sea multiplo de frame_size
	if((total_size % block_size) != 0){
		//Devuelve false si no son multiplos
		return false;
	}

	//Reservo memoria para el bloque de procesamiento
	uint32_t num_of_blocks = total_size / block_size;
	current_block = pvPortMalloc(FLOAT_SIZE_BYTES(block_size));
	if(current_block == NULL){
		return false;
	}

	for(uint8_t i = 0; i < num_of_blocks; i++){
		//Convierto los valores obtenidos a tension
		get_voltage(&voice_buffer[i * block_size], current_block, block_size);

		//Guardo en la SD
		save_buffer_on_sd(filename, current_block, block_size);
	}

	//Libero memoria utilizada
	vPortFree(current_block);
	return true;											//Ejecucion exitosa de la funcion
}

bool capture_and_store_voice(char *filename){
	//Declaracion de variables
	uint16_t *voice_buffer = NULL;
	display_message_t message;
	bool process_flag;
	//Reservo espacio para el buffer de voz
	voice_buffer = pvPortMalloc(U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
	if(voice_buffer == NULL){
		return false;
	}

	//Envio mensaje al display para indicar que comienza reconocimiento de voz
	message = DISPLAY_START_SPEECH_REC;
	xQueueSend(display_queue, &message, portMAX_DELAY);									//Envio mensaje al display
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);

	//Capturo la voz
	capture_voice(voice_buffer, AUDIO_BUFFER_SIZE);

	//Muestro en pantalla que se estan procesando los datos
	message = DISPLAY_PROCESSING_DATA;
	xQueueSend(display_queue, &message, portMAX_DELAY);



	//Guardo señal de audio convertida en tension en la memoria SD
	process_flag = (!store_voice(voice_buffer, AUDIO_BUFFER_SIZE, BLOCK_SIZE, filename)) ? false : true;
	vPortFree(voice_buffer);
	return process_flag;
}




