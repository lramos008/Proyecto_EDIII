#include "utils.h"
#include "sd_functions.h"
/*================[Public functions]=====================*/
void capture_voice(uint16_t *buffer, uint32_t size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, size);
	return;
}

void get_voltage(uint16_t *in_buffer, float *out_buffer, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out_buffer[i] = VOLTAGE_REFERENCE * ((float)in_buffer[i]) / ADC_RESOLUTION;
	}
}

void capture_and_store_voice(uint16_t *voice_buffer, uint32_t size, char *filename){
	indicatorMessage current_message;

	//Capturo voz
	capture_voice(voice_buffer, size);
	while(!conv_cplt_flag);
	conv_cplt_flag = false;

	//Aviso al display que termino la captura y empieza el procesamiento
	current_message = PANTALLA_PROCESANDO_DATOS;
	xQueueSend(display_queue, &current_message, portMAX_DELAY);

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
	float *voices[TEMPLATE_SAMPLES];
	char filenames[TEMPLATE_SAMPLES][DIR_STR_SIZE] = {"voice_1.bin", "voice_2.bin", "voice_3.bin", "voice_4.bin", "voice_5.bin"};

	//Reservo memoria para el template y para el buffer auxiliar
	template = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE / 2));
	aux = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE / 2));

	//Reservo memoria para cada buffer de voz
	for(uint8_t i = 0; i < TEMPLATE_SAMPLES; i++){
		voices[i] = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE));
	}

	//Inicializo bloque de template con ceros
	arm_fill_f32(0.0f, template, BLOCK_SIZE / 2);

	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Leo un bloque de cada archivo
		for(uint8_t j = 0; j < TEMPLATE_SAMPLES; j++){
			read_buffer_from_sd(filenames[j], voices[j], BLOCK_SIZE, i * BLOCK_SIZE);
		}

		//Proceso cada bloque de las diferentes voces y lo sumo al template
		for(uint8_t j = 0; j < TEMPLATE_SAMPLES; j++){
			process_signal(voices[j], aux, BLOCK_SIZE);
			arm_add_f32(template, aux, template, BLOCK_SIZE / 2);
			arm_fill_f32(0.0f, aux, BLOCK_SIZE / 2);								//Reseteo para el siguiente procesamiento
		}

		//Escalo para obtener el promedio
		arm_scale_f32(template, 1.0f / TEMPLATE_SAMPLES, template, BLOCK_SIZE / 2);

		//Guardo bloque de template en la SD
		save_buffer_on_sd("current_template.bin", template, BLOCK_SIZE / 2);
	}

	//Libero memoria alocada dinamicamente para las voces
	for(uint8_t i = 0; i < TEMPLATE_SAMPLES; i++){
		vPortFree(voices[i]);
	}

	//Libero memoria utilizada por template y aux
	vPortFree(template);
	vPortFree(aux);
	return;
}
