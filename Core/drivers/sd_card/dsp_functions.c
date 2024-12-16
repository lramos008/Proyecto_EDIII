#include "dsp_functions.h"

#define NUM_TAPS 50
#define STATE_SIZE (NUM_TAPS + FRAME_SIZE - 1)
#define THRESHOLD 0.1f									//Threshold entre bins
#define	BIN_PERCENTAGE_THRESHOLD 0.8f					//Threshold de porcentaje de bins correctos
/*=====================[Private variables]=======================*/
arm_fir_instance_f32 fir_instance;
arm_rfft_fast_instance_f32 fft_instance;
/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 12000 Hz

* 0 Hz - 100 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -45.06015749030636 dB

* 400 Hz - 3500 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 2.3015718508000034 dB

* 4000 Hz - 6000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -45.06015749030636 dB

*/
//Se colocaron los coeficientes obtenidos en orden inverso, pues la biblioteca cmsis dsp asi los necesita
const float32_t filter_taps[NUM_TAPS] = { 0.025663017507547244, 0.043161199365629975, 0.009987413560588994, -0.019344649278080953, 0.003579388809249739,
		 	 	 	 	 	 	 	 	  0.009067929219325404, -0.01872037528825959, -0.007413256377189857, 0.006276987439723166, -0.02485046961359185,
										  -0.02205694447886603, 0.0028270126760762916, -0.03125345871490819, -0.04154747028815444, -0.0014112320202949225,
										  -0.03404755206589369, -0.06602492769687618, -0.00582972886335776, -0.027505345684807933, -0.0989845714402987,
										  -0.009438922328850387, 0.006415464554856044, -0.17166579577787455, -0.011509402813520562, 0.48183338925387226,
										  0.48183338925387226, -0.011509402813520562, -0.17166579577787455, 0.006415464554856044, -0.009438922328850387,
										  -0.0989845714402987, -0.027505345684807933, -0.00582972886335776, -0.06602492769687618, -0.03404755206589369,
										  -0.0014112320202949225, -0.04154747028815444, -0.03125345871490819, 0.0028270126760762916, -0.02205694447886603,
										  -0.02485046961359185, 0.006276987439723166, -0.007413256377189857, -0.01872037528825959, 0.009067929219325404,
										  0.003579388809249739, -0.019344649278080953, 0.009987413560588994, 0.043161199365629975, 0.025663017507547244   };

/*Defino instancias para el filtro fir y la fft*/

static void fir_filter(float32_t *psrc, float32_t *pdst, const float32_t *coeff, uint16_t num_coeff, size_t block_size){
	static uint8_t fir_init_flag = 0;
	float32_t *fir_state = pvPortMalloc(STATE_SIZE * sizeof(float32_t));
	if(!fir_init_flag){
		arm_fir_init_f32(&fir_instance, num_coeff, (float32_t *)coeff, fir_state, block_size);
		fir_init_flag = 1;
	}
	arm_fir_f32(&fir_instance, psrc, pdst, block_size);
	vPortFree(fir_state);
	fir_state = NULL;
	return;
}

static void hamming_window(float32_t *psrc, size_t block_size){
	// Constantes para la ventana de hamming
	const float32_t alpha = 0.54f;
	const float32_t beta = 0.46f;
	float32_t hamming_coeff;
	for(size_t i = 0; i < block_size; i++){
		// Calculo el coeficiente de hamming para la muestra actual
		hamming_coeff = alpha - beta * arm_cos_f32((2.0f * PI * i) / (block_size - 1));
		// Aplico coeficiente de hamming a la muestra de la señal
		psrc[i] *= hamming_coeff;
	}
	return;
}

static void calculate_fft(float32_t *psrc, float32_t *pdst, size_t block_size){
	static uint8_t fft_init_flag = 0;
	/*Inicializo la instancia de fft si corresponde*/
	if(!fft_init_flag){
		arm_rfft_fast_init_f32(&fft_instance, block_size);
		fft_init_flag = 1;
	}
	/*Calculo la fft*/
	arm_rfft_fast_f32(&fft_instance, psrc, pdst, 0);					//El cero indica que se hace transformada, no antitransformada
	return;
}

static void normalize_array(float32_t *psrc, size_t block_size){
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

static void get_fft_norm_mag(float32_t *psrc, float32_t *pdst, size_t block_size){
	/*Se tienen en cuenta la cantidad de numeros complejos, no de elementos en p_src*/
	/*Calculo la magnitud de la fft*/
	arm_cmplx_mag_f32(psrc, pdst, block_size);
	/*Normalizo*/
	normalize_array(pdst, block_size);
	return;
}

void process_signal(float32_t *psrc, float32_t *pdst, size_t block_size){
	/*Reservo memoria para salida filtrada y filtro la señal.*/
	float32_t *filtered = pvPortMalloc(block_size * sizeof(float32_t));
	fir_filter(psrc, filtered, filter_taps, NUM_TAPS, block_size);

	//Aplico ventana de hamming al vector
	hamming_window(filtered, block_size);

	/*Reservo memoria para la salida de FFT y calculo la fft. Libero memoria de salida filtrada*/
	float32_t *fft_output = pvPortMalloc(block_size * sizeof(float32_t));
	calculate_fft(filtered, fft_output, block_size);
	vPortFree(filtered);
	filtered = NULL;

	/*Calculo la magnitud de la fft y libero la memoria ocupada por la fft.*/
	get_fft_norm_mag(fft_output, pdst, block_size / 2);
	vPortFree(fft_output);
	fft_output = NULL;
	return;
}

uint8_t compare_features(float32_t *feature_1, float32_t *feature_2, size_t length){
	size_t bins_within_threshold = 0;
	float32_t bins_percentage;
	float32_t *diff = pvPortMalloc(length * sizeof(float32_t));
	float32_t *abs_diff = pvPortMalloc(length * sizeof(float32_t));
	/*Resto los feature y calculo el valor absoluto*/
	arm_sub_f32(feature_1, feature_2, diff, length);
	arm_abs_f32(diff, abs_diff, length);
	/*Cuento cuantos bins estan por debajo del threshold*/
	for(size_t i = 0; i < length; i++){
		if(abs_diff[i] <= THRESHOLD){
			bins_within_threshold++;
		}
	}
	bins_percentage = ((float32_t) bins_within_threshold) / length;
	/*Libero la memoria utilizada*/
	vPortFree(diff);
	vPortFree(abs_diff);
	/*Chequeo si el porcentaje de bins aceptados supera el threshold*/
	if(bins_percentage >= BIN_PERCENTAGE_THRESHOLD){
		return 1;										//Bloque reconocido
	}
	else{
		return 0;										//Bloque no reconocido
	}
}

