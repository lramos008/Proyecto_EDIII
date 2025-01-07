#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

static bool calculate_fft(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Declaracion de variables
	arm_rfft_fast_instance_f32 fft_instance;							//Instancia necesaria para realizar fft
	arm_status status;													//Para comprobar resultado de operaciones

	//Inicializo instancia de fft
	status = arm_rfft_fast_init_f32(&fft_instance, size);
	if(status != ARM_MATH_SUCCESS){
		//Error al inicializar instancia de fft
		return false;
	}

	/*Calculo la fft*/
	arm_rfft_fast_f32(&fft_instance, psrc, pdst, 0);					//El cero indica que se hace transformada, no antitransformada
	return true;
}

static void calculate_magnitude(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Calculo el valor absoluto de la fft
	arm_cmplx_mag_f32(psrc, pdst, size);
	return;
}

static void normalize_array(float32_t *psrc, uint32_t size){
	//Declaracion de variables
	float32_t min_val, max_val;
	uint32_t min_index, max_index;
	float32_t range;

	/*Encuentro el maximo y el minimo del array*/
	arm_min_f32(psrc, size, &min_val, &min_index);
	arm_max_f32(psrc, size, &max_val, &max_index);

	/*Calculo el rango*/
	range = max_val - min_val;

	/*Normalizo el array*/
	for(size_t i = 0; i < size; i++){
		psrc[i] = (psrc[i] - min_val) / range;
	}
	return;
}

bool get_normalized_fft_magnitude(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Declaracion de variables
	float32_t *fft_array = NULL;

	//Reservo memoria para los vectores
	fft_array = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	if(fft_array == NULL){
		return false;													//Operacion fallida
	}

	//Calculo la fft
	if(!calculate_fft(psrc, fft_array, size)){
		//Si hubo un error en la operacion de la fft, devuelve false
		return false;
	}

	//Calculo magnitud de la fft
	calculate_magnitude(fft_array, pdst, size / 2);						//Guardo resultado en vector destino

	//Normalizo array
	normalize_array(pdst, size / 2);

	//Libero memoria
	vPortFree(fft_array);
	return true;
}





