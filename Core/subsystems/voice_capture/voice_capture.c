#include "common_utils.h"
#include "sd_functions.h"
#include "save_and_read_data.h"
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
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

static bool save_voice(char *voice_path, uint16_t *voice_buffer, uint32_t total_size, uint32_t block_size){
	//Declaro vector de procesamiento
	float32_t voice_block[BLOCK_SIZE] = {0};
	uint32_t num_of_blocks;
	bool res;

	//Verifico que el tamaño total sea multiplo del tamaño del bloque
	if(total_size % block_size != 0){
		return false;
	}

	//Calculo numero de bloques
	num_of_blocks = total_size / block_size;

	//Convierto la voz a float de a bloques
	for(uint8_t i = 0; i < num_of_blocks; i++){
		//Convierto cuentas del adc a valores de tension
		get_voltage(&voice_buffer[i * block_size], voice_block, block_size);

		//Guardo bloque
		res = save_data_on_sd(voice_path, (void *)voice_block, FLOAT_SIZE_BYTES(block_size));
		if(!res){
			send_error(DISPLAY_WRITE_SD_ERROR);							//Envio el error al display
			return false;
		}
	}

	//Si se llego hasta aca, la operacion de guardado fue un exito
	return true;
}

bool capture_voice_signal(char *voice_path){
	//Declaracion de variables
	display_message_t message;
	bool res;
	uint16_t voice_buffer[AUDIO_BUFFER_SIZE] = {0};

	//Envio mensaje al display para indicar que comienza reconocimiento de voz
	message = DISPLAY_START_SPEECH_REC;
	xQueueSend(display_queue, &message, portMAX_DELAY);									//Envio mensaje al display
	xSemaphoreTake(sd_display_sync, portMAX_DELAY);

	//Capturo voz
	capture_voice(voice_buffer, AUDIO_BUFFER_SIZE);

	//Muestro en pantalla que se estan procesando los datos
	message = DISPLAY_PROCESSING_DATA;
	xQueueSend(display_queue, &message, portMAX_DELAY);

	//Almaceno la voz en la memoria SD
	res = save_voice(voice_path, voice_buffer, AUDIO_BUFFER_SIZE, BLOCK_SIZE);
	if(!res){														//Borro el archivo creado parcialmente
		return false;
	}

	//Voz capturada con exito
	return true;
}

