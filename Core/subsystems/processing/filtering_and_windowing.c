#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

/*=====================[Private defines]====================*/
#define NUM_TAPS 	101
#define STATE_SIZE 	(NUM_TAPS + BLOCK_SIZE - 1)

/*
 * Coeficientes para filtro fir pasa bandas
 * fc_inf = 300 Hz
 * fc_sup = 3400 Hz
 * num of taps = 101
 */
float32_t coefficients[] = {-0.000165f,     0.000532f,     0.000854f,     0.000359f,     -0.000149f,
						0.000326f,     0.001382f,     0.001558f,     0.000538f,     0.000024f,
						0.001337f,     0.002979f,     0.002447f,     0.000345f,     0.000179f,
						0.002980f,     0.004808f,     0.002328f,     -0.001245f,     -0.000129f,
						0.004604f,     0.005256f,     -0.000788f,     -0.005450f,     -0.001471f,
						0.005015f,     0.002105f,     -0.008842f,     -0.012817f,     -0.003828f,
						0.003183f,     -0.006564f,     -0.022709f,     -0.022230f,     -0.005689f,
						-0.000979f,     -0.021868f,     -0.041819f,     -0.030133f,     -0.002845f,
						-0.006299f,     -0.045843f,     -0.066101f,     -0.028542f,     0.017379f,
						-0.010767f,     -0.099137f,     -0.116546f,     0.035003f,     0.272081f,
						0.387823f,     0.272081f,     0.035003f,     -0.116546f,     -0.099137f,
						-0.010767f,     0.017379f,     -0.028542f,     -0.066101f,     -0.045843f,
						-0.006299f,     -0.002845f,     -0.030133f,     -0.041819f,     -0.021868f,
						-0.000979f,     -0.005689f,     -0.022230f,     -0.022709f,     -0.006564f,
						0.003183f,     -0.003828f,     -0.012817f,     -0.008842f,     0.002105f,
						0.005015f,     -0.001471f,     -0.005450f,     -0.000788f,     0.005256f,
						0.004604f,     -0.000129f,     -0.001245f,     0.002328f,     0.004808f,
						0.002980f,     0.000179f,     0.000345f,     0.002447f,     0.002979f,
						0.001337f,     0.000024f,     0.000538f,     0.001558f,     0.001382f,
						0.000326f,     -0.000149f,     0.000359f,     0.000854f,     0.000532f,
						-0.000165f																	};

static bool filter_signal(float32_t *psrc, float32_t *pdst, uint32_t size){
	/*Es necesaria la instancia y el vector de estado para usar el filtro FIR*/
	arm_fir_instance_f32 fir_instance;											//Instancia del filtro fir
	float32_t *state_vector = NULL;

	//Reservo memoria para el vector de estado
	state_vector = pvPortMalloc(FLOAT_SIZE_BYTES(STATE_SIZE));
	if(state_vector == NULL){
		//No se pudo reservar la memoria para el vector de estado, devuelve false
		return false;
	}

	//Inicializo instancia de filtro fir predeterminada
	arm_fir_init_f32(&fir_instance, NUM_TAPS, (float32_t *)coefficients, state_vector, size);

	//Filtro la señal
	arm_fir_f32(&fir_instance, psrc, pdst, size);

	//Libero memoria utilizada
	vPortFree(state_vector);
	return true;										//Filtrado realizado con exito
}

static bool apply_windowing(float32_t *psrc, uint32_t size){

	// Constantes para la ventana de hamming
	const float32_t alpha = 0.54f;
	const float32_t beta = 0.46f;
	float32_t hamming_coeff;

	for(uint32_t i = 0; i < size; i++){
		// Calculo el coeficiente de hamming para la muestra actual
		hamming_coeff = alpha - beta * arm_cos_f32((2.0f * PI * i) / (size - 1));

		// Aplico coeficiente de hamming a la muestra de la señal
		psrc[i] *= hamming_coeff;
	}
	return true;
}

bool filter_and_windowing(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Si alguna de las 2 funciones me devuelve false por algun error, la funcion general devuelve false
	return (filter_signal(psrc, pdst, size) && apply_windowing(pdst, size));
}


