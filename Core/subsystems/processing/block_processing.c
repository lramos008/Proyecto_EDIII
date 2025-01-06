#include "processing_functions.h"

bool process_block(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Declaracion de variables
	float32_t *filtered_signal = NULL;
	bool process_flag;

	//Reservo memoria para la señal filtrada
	filtered_signal = pvPortMalloc(FLOAT_SIZE_BYTES(size));

	//Filtro la señal
	if(!filter_and_windowing(psrc, filtered_signal, size) || !get_normalized_fft_magnitude(filtered_signal, pdst, size)){
		process_flag = false;
	}
	else{
		process_flag = true;
	}
	vPortFree(filtered_signal);
	return process_flag;					//Operacion exitosa
}
