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

void extract_and_save_features(char *voice_name, char *feature_name){
	float *current_block = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE));
	float *feature_block = pvPortMalloc(FLOAT_SIZE_BYTES(FFT_NORM_MAG_SIZE));
	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		read_buffer_from_sd(voice_name, current_block, BLOCK_SIZE, i * BLOCK_SIZE);
		process_signal(current_block, feature_block, BLOCK_SIZE);
		save_buffer_on_sd(feature_name, feature_block, FFT_NORM_MAG_SIZE);
	}

	//Elimino el archivo de voz para quedarme solo con los features
	f_unlink(voice_name);

	//Libero memoria utilizada
	vPortFree(current_block);
	vPortFree(feature_block);
	return;
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
	arm_fill_f32(0.0f, aux, FFT_NORM_MAG_SIZE);

	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//En cada iteracion se leen los bloques de todas las voces, se procesan y luego se procesan las magnitudes de fft normalizadas
		for(uint8_t j = 0; j < NUM_OF_SAMPLES; j++){
			read_buffer_from_sd(filenames[j], voices[j], BLOCK_SIZE, i * BLOCK_SIZE);		//Se lee el bloque i de la voz voice_j.bin
			process_signal(voices[j], aux, BLOCK_SIZE);										//Proceso voz para obtener feature deseada
			arm_add_f32(template, aux, template, FFT_NORM_MAG_SIZE);						//Sumo elemento a elemento al vector template
			arm_fill_f32(0.0f, aux, FFT_NORM_MAG_SIZE);
		}

		//Escalo para obtener el promedio
		arm_scale_f32(template, 1.0f / NUM_OF_SAMPLES, template, FFT_NORM_MAG_SIZE);

		//Guardo bloque de template en la SD
		save_buffer_on_sd("current_template.bin", template, FFT_NORM_MAG_SIZE);
	}

	//Borro archivos generados para crear el template y libero memoria
	for(uint8_t i = 0; i < NUM_OF_SAMPLES; i++){
		f_unlink(filenames[i]);																//Borro archivos creados para generar template
		vPortFree(voices[i]);																//Libero memoria usada para cada una de las voces
	}
	vPortFree(template);
	vPortFree(aux);
	return;
}

bool compare_features(float32_t *feature_1, float32_t *feature_2, uint32_t size){

}

bool check_voice(char *template_path, char *feature_path){
	float32_t *template = pvPortMalloc(FLOAT_SIZE_BYTES(FFT_NORM_MAG_SIZE));
	float32_t *extracted_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FFT_NORM_MAG_SIZE));
	bool compare_res = false;
	bool is_recognized = false;
	uint8_t block_counter = 0;
	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Leo cada bloque del archivo, y comparo bin a bin
		read_buffer_from_sd(template_path, template, FFT_NORM_MAG_SIZE, i * FFT_NORM_MAG_SIZE);
		read_buffer_from_sd(feature_path, extracted_feature, FFT_NORM_MAG_SIZE, i * FFT_NORM_MAG_SIZE);
		compare_res = compare_features(extracted_feature, template, FFT_NORM_MAG_SIZE);
		if(compare_res == false){
			break;
		}
		block_counter++;																	//Aumento el conteo de bloques correctos
	}

	//Compruebo si hay NUM_OF_BLOCKS bloques correctos
	is_recognized = (block_counter < NUM_OF_BLOCKS) ? false : true;
	return is_recognized;																	//Devuelvo estado del reconocimiento
}


