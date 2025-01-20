#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

/*=====================[Private defines]====================*/
#define NUM_TAPS 	401
#define STATE_SIZE 	(NUM_TAPS + BLOCK_SIZE - 1)


/*
 * Coeficientes para filtro fir pasa bandas
 * fc_inf = 300 Hz
 * fc_sup = 3400 Hz
 * num of taps = 401
 */
float fir_coefficients[] = {
    0.000127, 0.000252, 0.000184, 0.000023, 0.000011, 0.000161, 0.000235, 0.000103, -0.000050, -0.000005,
    0.000156, 0.000164, -0.000023, -0.000146, -0.000037, 0.000116, 0.000046, -0.000179, -0.000243, -0.000068,
    0.000053, -0.000103, -0.000334, -0.000307, -0.000074, -0.000012, -0.000248, -0.000443, -0.000297, -0.000026,
    -0.000053, -0.000351, -0.000456, -0.000175, 0.000093, -0.000049, -0.000371, -0.000336, 0.000069, 0.000276,
    0.000000, -0.000294, -0.000078, 0.000406, 0.000478, 0.000068, -0.000136, 0.000272, 0.000754, 0.000616,
    0.000099, 0.000052, 0.000627, 0.000995, 0.000593, 0.000030, 0.000198, 0.000875, 0.001008, 0.000329,
    -0.000179, 0.000243, 0.000917, 0.000713, -0.000191, -0.000515, 0.000171, 0.000715, 0.000117, -0.000887,
    -0.000883, 0.000028, 0.000311, -0.000665, -0.001577, -0.001125, -0.000078, -0.000174, -0.001429, -0.002021,
    -0.001055, -0.000022, -0.000575, -0.001934, -0.001984, -0.000537, 0.000279, -0.000755, -0.001984, -0.001336,
    0.000439, 0.000806, -0.000661, -0.001508, -0.000116, 0.001708, 0.001402, -0.000370, -0.000604, 0.001431,
    0.002933, 0.001792, -0.000069, 0.000477, 0.002901, 0.003675, 0.001659, 0.000005, 0.001403, 0.003838,
    0.003528, 0.000775, -0.000322, 0.001882, 0.003878, 0.002274, -0.000880, -0.001050, 0.001783, 0.002891,
    0.000000, -0.003024, -0.001951, 0.001203, 0.001054, -0.002850, -0.005087, -0.002583, 0.000462, -0.001164,
    -0.005551, -0.006321, -0.002418, -0.000008, -0.003137, -0.007275, -0.006016, -0.001035, 0.000157, -0.004275,
    -0.007336, -0.003752, 0.001661, 0.001066, -0.004245, -0.005429, 0.000388, 0.005284, 0.002435, -0.003119,
    -0.001786, 0.005714, 0.008928, 0.003577, -0.001391, 0.002823, 0.010995, 0.011298, 0.003548, 0.000146,
    0.007255, 0.014692, 0.010992, 0.001413, 0.000673, 0.010289, 0.015310, 0.006860, -0.003406, -0.000329,
    0.010978, 0.011754, -0.001658, -0.010825, -0.002766, 0.008950, 0.003576, -0.014336, -0.019885, -0.005698,
    0.004554, -0.009053, -0.030256, -0.028675, -0.007119, -0.001192, -0.025920, -0.048385, -0.034099, -0.003154,
    -0.006858, -0.049087, -0.069742, -0.029726, 0.017899, -0.010985, -0.100366, -0.117287, 0.035075, 0.271950,
    0.387307, 0.271950, 0.035075, -0.117287, -0.100366, -0.010985, 0.017899, -0.029726, -0.069742, -0.049087,
    -0.006858, -0.003154, -0.034099, -0.048385, -0.025920, -0.001192, -0.007119, -0.028675, -0.030256, -0.009053,
    0.004554, -0.005698, -0.019885, -0.014336, 0.003576, 0.008950, -0.002766, -0.010825, -0.001658, 0.011754,
    0.010978, -0.000329, -0.003406, 0.006860, 0.015310, 0.010289, 0.000673, 0.001413, 0.010992, 0.014692,
    0.007255, 0.000146, 0.003548, 0.011298, 0.010995, 0.002823, -0.001391, 0.003577, 0.008928, 0.005714,
    -0.001786, -0.003119, 0.002435, 0.005284, 0.000388, -0.005429, -0.004245, 0.001066, 0.001661, -0.003752,
    -0.007336, -0.004275, 0.000157, -0.001035, -0.006016, -0.007275, -0.003137, -0.000008, -0.002418, -0.006321,
    -0.005551, -0.001164, 0.000462, -0.002583, -0.005087, -0.002850, 0.001054, 0.001203, -0.001951, -0.003024,
    0.000000, 0.002891, 0.001783, -0.001050, -0.000880, 0.002274, 0.003878, 0.001882, -0.000322, 0.000775,
    0.003528, 0.003838, 0.001403, 0.000005, 0.001659, 0.003675, 0.002901, 0.000477, -0.000069, 0.001792,
    0.002933, 0.001431, -0.000604, -0.000370, 0.001402, 0.001708, -0.000116, -0.001508, -0.000661, 0.000806,
    0.000439, -0.001336, -0.001984, -0.000755, 0.000279, -0.000537, -0.001984, -0.001934, -0.000575, -0.000022,
    -0.001055, -0.002021, -0.001429, -0.000174, -0.000078, -0.001125, -0.001577, -0.000665, 0.000311, 0.000028,
    -0.000883, -0.000887, 0.000117, 0.000715, 0.000171, -0.000515, -0.000191, 0.000713, 0.000917, 0.000243,
    -0.000179, 0.000329, 0.001008, 0.000875, 0.000198, 0.000030, 0.000593, 0.000995, 0.000627, 0.000052,
    0.000099, 0.000616, 0.000754, 0.000272, -0.000136, 0.000068, 0.000478, 0.000406, -0.000078, -0.000294,
    0.000000, 0.000276, 0.000069, -0.000336, -0.000371, -0.000049, 0.000093, -0.000175, -0.000456, -0.000351,
    -0.000053, -0.000026, -0.000297, -0.000443, -0.000248, -0.000012, -0.000074, -0.000307, -0.000334, -0.000103,
    0.000053, -0.000068, -0.000243, -0.000179, 0.000046, 0.000116, -0.000037, -0.000146, -0.000023, 0.000164,
    0.000156, -0.000005, -0.000050, 0.000103, 0.000235, 0.000161, 0.000011, 0.000023, 0.000184, 0.000252,
    0.000127
};


bool filter_signal(float32_t *psrc, float32_t *pdst, uint32_t size){

	/*Es necesaria la instancia y el vector de estado para usar el filtro FIR*/
	arm_fir_instance_f32 fir_instance;											//Instancia del filtro fir
	float32_t *state_vector = NULL;

	//Reservo memoria para el vector de estado
	state_vector = pvPortMalloc(FLOAT_SIZE_BYTES(STATE_SIZE));
	if(state_vector == NULL){
		//Manejo error al reservar memoria
		send_error(DISPLAY_MEMORY_ERROR);
		return false;
	}

	//Inicializo vector de estados con ceros
	arm_fill_f32(0.0f, state_vector, STATE_SIZE);

	//Inicializo instancia de filtro fir predeterminada
	arm_fir_init_f32(&fir_instance, NUM_TAPS, (float32_t *)coefficients, state_vector, size);

	//Filtro la señal
	arm_fir_f32(&fir_instance, psrc, pdst, size);

	//Libero memoria utilizada
	vPortFree(state_vector);
	return true;										//Filtrado realizado con exito
}

bool apply_windowing(float32_t *psrc, uint32_t size){

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


