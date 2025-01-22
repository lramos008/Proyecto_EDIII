#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

bool initialize_fft(arm_rfft_fast_instance_f32 *rfft_instance, uint32_t block_size){
	arm_status status;
	status = arm_rfft_fast_init_f32(rfft_instance, block_size);
	if(status != ARM_MATH_SUCCESS){
		return false;											//Error al inicializar instancia de fft
	}
	return true;
}

void calculate_rfft(arm_rfft_fast_instance_f32 *rfft_instance, float32_t *psrc, float32_t *pdst){
	/*Calculo la fft*/
	arm_rfft_fast_f32(rfft_instance, psrc, pdst, 0);					//El cero indica que se hace transformada, no antitransformada
	return;
}


void calculate_magnitude(float32_t *psrc, float32_t *pdst, uint32_t block_size){
	//Calculo el valor absoluto de la fft
	arm_cmplx_mag_f32(psrc, pdst, block_size);
	return;
}

void normalize_array(float32_t *psrc, uint32_t block_size){
	//Declaracion de variables
	float32_t min_val, max_val;
	uint32_t min_index, max_index;
	float32_t range;

	/*Encuentro el maximo y el minimo del array*/
	arm_min_f32(psrc, block_size, &min_val, &min_index);
	arm_max_f32(psrc, block_size, &max_val, &max_index);

	/*Calculo el rango*/
	range = max_val - min_val;

	/*Normalizo el array*/
	for(size_t i = 0; i < block_size; i++){
		psrc[i] = (psrc[i] - min_val) / range;
	}
	return;
}

