#include "dsp_functions.h"
/*=====================[Extern variables]========================*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern uint8_t conv_cplt_flag;
/*=====================[Private defines]=========================*/
#define ADC_RESOLUTION 4096
#define VOLTAGE_REFERENCE 3.3f
#define NUM_TAPS 100
#define STATE_SIZE (NUM_TAPS + FRAME_SIZE - 1)
#define THRESHOLD 0.1f
#define	BIN_PERCENTAGE_THRESHOLD 0.8f
/*=====================[Private variables]=======================*/
arm_fir_instance_f32 fir_instance;
arm_rfft_fast_instance_f32 fft_instance;
const float32_t fir_coeff[NUM_TAPS] =
				{ 0.011382947313982183, 0.007278960905837157, -0.006998062915062974, -0.01203024080070622, 0.006178892093241564,
				  0.02652850663025597, 0.02332814015761635, 0.00720967093499501, 0.003874921882229113, 0.009214774027249427,
				  0.0029841400362301635, -0.008634063062966789, -0.00574221019882645, 0.004013259203023682, 0.00007839824056997462,
				  -0.009225910167748234, -0.0037379046374667537, 0.005926664706238162, -0.0011544947403069354, -0.01226263399042312,
				  -0.0055742028639393145, 0.00394247940742394, -0.006217083187201803, -0.018253966782994423, -0.008631839895822805,
				  0.0017142896845039116, -0.011791198897201458, -0.024596929631868294, -0.011040183660379834, 0.00012220764355865894,
				  -0.018002431036377536, -0.03171499562847049, -0.012749934889886252, -0.000698807729755436, -0.025373295764521756,
				  -0.03996929589125958, -0.012730954552648721, 0.0005541247857911273, -0.034402622521298575, -0.05015576787247386,
				  -0.008711180157744471, 0.006649563124164676, -0.04810422073146836, -0.06669536801210646, 0.006590491265364713,
				  0.028816836721552816, -0.08463900603542567, -0.12422680797956051, 0.11508617612207177, 0.4287639166258248,
				  0.4287639166258248, 0.11508617612207177, -0.12422680797956051, -0.08463900603542567, 0.028816836721552816,
				  0.006590491265364713, -0.06669536801210646, -0.04810422073146836, 0.006649563124164676, -0.008711180157744471,
				  -0.05015576787247386, -0.034402622521298575, 0.0005541247857911273, -0.012730954552648721, -0.03996929589125958,
				  -0.025373295764521756, -0.000698807729755436, -0.012749934889886252, -0.03171499562847049, -0.018002431036377536,
				  0.00012220764355865894, -0.011040183660379834, -0.024596929631868294, -0.011791198897201458, 0.0017142896845039116,
				  -0.008631839895822805, -0.018253966782994423, -0.006217083187201803, 0.00394247940742394, -0.0055742028639393145,
				  -0.01226263399042312, -0.0011544947403069354, 0.005926664706238162, -0.0037379046374667537, -0.009225910167748234,
				  0.00007839824056997462, 0.004013259203023682, -0.00574221019882645, -0.008634063062966789, 0.0029841400362301635,
				  0.009214774027249427, 0.003874921882229113, 0.00720967093499501, 0.02332814015761635, 0.02652850663025597,
				  0.006178892093241564, -0.01203024080070622, -0.006998062915062974, 0.007278960905837157, 0.011382947313982183 };

/*Defino instancias para el filtro fir y la fft*/

void capture_voice(uint16_t *buffer, uint16_t buf_size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, buf_size);
	while(conv_cplt_flag == 0);
	conv_cplt_flag = 0;
	return;
}




static void get_voltage(uint16_t *psrc, float32_t *pdst, size_t block_size){
	for(size_t i = 0; i < block_size; i++){
		pdst[i] = ((float) psrc[i]) * VOLTAGE_REFERENCE / ADC_RESOLUTION;
	}
	return;
}

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

void process_signal(uint16_t *psrc, float32_t *pdst, size_t block_size){
	/*Reservo memoria y realizo conversion a tension*/
	float32_t *voltage_conversion = pvPortMalloc(block_size * sizeof(float32_t));
	get_voltage(psrc, voltage_conversion, block_size);
	/*Reservo memoria para salida filtrada y filtro la señal. Libero memoria no utilizada.*/
	float32_t *filtered = pvPortMalloc(block_size * sizeof(float32_t));
	fir_filter(voltage_conversion, filtered, fir_coeff, NUM_TAPS, block_size);
	vPortFree(voltage_conversion);													//Libero memoria
	voltage_conversion = NULL;														//Me aseguro que no se produzca un dangling pointer
	/*Aplico ventana de hamming a la salida filtrada*/
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
		return 1;
	}
	else{
		return 0;
	}
}

