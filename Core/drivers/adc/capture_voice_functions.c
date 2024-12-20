#include "utils.h"
#include "sd_functions.h"
#define ARM_MATH_CM4
#include "arm_math.h"
#include "dsp_functions.h"
/*================[Public functions]=====================*/
void capture_voice(uint16_t *buffer, uint32_t size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, size);
	while(!conv_cplt_flag);
	conv_cplt_flag = false;
	return;
}

void get_voltage(uint16_t *in_buffer, float *out_buffer, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out_buffer[i] = VOLTAGE_REFERENCE * ((float)in_buffer[i]) / ADC_RESOLUTION;
	}
}

void store_voice(uint16_t *voice_buffer, uint32_t size, char *filename){
	//Reservo memoria para el bloque de procesamiento
	float *current_block = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE));
	if(current_block == NULL){
		//Manejar este caso, enviar un print
		return;
	}

	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Convierto los valores obtenidos a tension
		get_voltage(&voice_buffer[i * BLOCK_SIZE], current_block, BLOCK_SIZE);

		//Guardo en la SD
		save_buffer_on_sd(filename, current_block, BLOCK_SIZE);
	}

	//Libero memoria utilizada
	vPortFree(current_block);
}

void generate_template(void){
	float *template, *aux;
	float *voices[NUM_OF_SAMPLES];
	char filenames[NUM_OF_SAMPLES][15] = {"voice_1.bin", "voice_2.bin", "voice_3.bin", "voice_4.bin", "voice_5.bin"};

	//Reservo memoria para el template y para el buffer auxiliar
	template = pvPortMalloc(FLOAT_SIZE_BYTES(FFT_NORM_MAG_SIZE));
	aux = pvPortMalloc(FLOAT_SIZE_BYTES(FFT_NORM_MAG_SIZE));

	//Reservo memoria para cada buffer de voz
	for(uint8_t i = 0; i < NUM_OF_SAMPLES; i++){
		voices[i] = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE));
	}

	//Inicializo bloque de template con ceros
	arm_fill_f32(0.0f, template, FFT_NORM_MAG_SIZE);

	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Lectura de voces desde archivo
		for(uint8_t j = 0; j < NUM_OF_SAMPLES; j++){
			//Leo el bloque j de cada uno de los archivos de voz, los procesos
			read_buffer_from_sd(filenames[j], voices[j], BLOCK_SIZE, i * BLOCK_SIZE);		//Se lee el bloque i de cada una de las voces
			process_signal(voices[j], aux, BLOCK_SIZE);
			arm_add_f32(template, aux, template, FFT_NORM_MAG_SIZE);
			arm_fill_f32(0.0f, aux, FFT_NORM_MAG_SIZE);
		}

		//Proceso cada bloque de las diferentes voces y lo sumo al template
		for(uint8_t j = 0; j < NUM_OF_SAMPLES; j++){
			process_signal(voices[j], aux, BLOCK_SIZE);
			arm_add_f32(template, aux, template, BLOCK_SIZE / 2);
			arm_fill_f32(0.0f, aux, BLOCK_SIZE / 2);										//Reseteo para el siguiente procesamiento
		}

		//Escalo para obtener el promedio
		arm_scale_f32(template, 1.0f / NUM_OF_SAMPLES, template, BLOCK_SIZE / 2);

		//Guardo bloque de template en la SD
		save_buffer_on_sd("current_template.bin", template, BLOCK_SIZE / 2);
	}

	//Borro archivos y libero memoria
	for(uint8_t i = 0; i < NUM_OF_SAMPLES; i++){
		f_unlink(filenames[i]);																//Borro archivos creados para generar template
		vPortFree(voices[i]);																//Libero memoria usada para cada una de las voces
	}
	vPortFree(template);
	vPortFree(aux);
	return;
}
