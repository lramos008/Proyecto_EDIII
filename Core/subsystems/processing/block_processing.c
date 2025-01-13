#include "processing_functions.h"

bool process_block(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Declaracion de variables
	float32_t *filtered_signal = NULL;
	bool process_flag;

	//Reservo memoria para la señal filtrada
	filtered_signal = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	if(filtered_signal == NULL){
		//Manejo error de memoria
		send_error(DISPLAY_MEMORY_ERROR);
		return false;
	}

	//Inicializo array con ceros
	arm_fill_f32(0.0f, filtered_signal, size);

	//Filtro la señal y calculo la magnitud normalizada de la fft
	process_flag = (!filter_and_windowing(psrc, filtered_signal, size) ||
					!get_normalized_fft_magnitude(filtered_signal, pdst, size)) ? false : true;
	vPortFree(filtered_signal);
	return process_flag;					//Resultado de la operacion
}
