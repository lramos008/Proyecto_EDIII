#include "utils.h"

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
