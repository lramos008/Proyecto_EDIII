#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

/*=====================[Private defines]====================*/
#define NUM_TAPS 	101
#define STATE_SIZE 	(NUM_TAPS + BLOCK_SIZE - 1)

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 16000 Hz

* 0 Hz - 300 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -37.9077351126332 dB

* 500 Hz - 3500 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = 1.9622035767787869 dB

* 3800 Hz - 8000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -37.9077351126332 dB

*/



const float coefficients[NUM_TAPS] = {
  0.014106940596366732,
  0.01259719692585461,
  0.00015605658197359883,
  -0.016523435347721167,
  -0.021767457349316597,
  -0.013393149099204733,
  -0.004069347786391853,
  -0.003115649846318564,
  -0.00607349518094268,
  -0.0052833895509432766,
  -0.0017641265999963237,
  -0.0003752644706929078,
  -0.00027567138910165623,
  0.0018367624449554573,
  0.004007166014296647,
  0.0030111422824517235,
  0.00221331836784153,
  0.006278033687740562,
  0.010970832607246761,
  0.00893006069344786,
  0.003934971892043095,
  0.0065023824008413,
  0.014804645091190165,
  0.015137002468859293,
  0.005467097332842773,
  0.001796975594926011,
  0.011459445501564004,
  0.01765995365210587,
  0.0063607832795682506,
  -0.007042007802329289,
  -0.0012099568286222922,
  0.012557617074767886,
  0.00570667325204919,
  -0.01754614837395077,
  -0.022365614939582147,
  -0.0025258698328908544,
  0.00304667630229688,
  -0.0254612954029277,
  -0.048137058112095996,
  -0.02808281714670458,
  -0.0010975445335978649,
  -0.02423861467963948,
  -0.07286615750415491,
  -0.06611173385299245,
  -0.005344333999175594,
  0.00155074426579704,
  -0.09067228640521426,
  -0.14709120095616318,
  -0.00801927737849216,
  0.2625708574543482,
  0.40285544237481163,
  0.2625708574543482,
  -0.00801927737849216,
  -0.14709120095616318,
  -0.09067228640521426,
  0.00155074426579704,
  -0.005344333999175594,
  -0.06611173385299245,
  -0.07286615750415491,
  -0.02423861467963948,
  -0.0010975445335978649,
  -0.02808281714670458,
  -0.048137058112095996,
  -0.0254612954029277,
  0.00304667630229688,
  -0.0025258698328908544,
  -0.022365614939582147,
  -0.01754614837395077,
  0.00570667325204919,
  0.012557617074767886,
  -0.0012099568286222922,
  -0.007042007802329289,
  0.0063607832795682506,
  0.01765995365210587,
  0.011459445501564004,
  0.001796975594926011,
  0.005467097332842773,
  0.015137002468859293,
  0.014804645091190165,
  0.0065023824008413,
  0.003934971892043095,
  0.00893006069344786,
  0.010970832607246761,
  0.006278033687740562,
  0.00221331836784153,
  0.0030111422824517235,
  0.004007166014296647,
  0.0018367624449554573,
  -0.00027567138910165623,
  -0.0003752644706929078,
  -0.0017641265999963237,
  -0.0052833895509432766,
  -0.00607349518094268,
  -0.003115649846318564,
  -0.004069347786391853,
  -0.013393149099204733,
  -0.021767457349316597,
  -0.016523435347721167,
  0.00015605658197359883,
  0.01259719692585461,
  0.014106940596366732
};



/*
 * Coeficientes para filtro fir pasa bandas
 * fc_inf = 300 Hz
 * fc_sup = 3400 Hz
 * num of taps = 101
 */
//float32_t coefficients[] = {-0.000165f,     0.000532f,     0.000854f,     0.000359f,     -0.000149f,
//						0.000326f,     0.001382f,     0.001558f,     0.000538f,     0.000024f,
//						0.001337f,     0.002979f,     0.002447f,     0.000345f,     0.000179f,
//						0.002980f,     0.004808f,     0.002328f,     -0.001245f,     -0.000129f,
//						0.004604f,     0.005256f,     -0.000788f,     -0.005450f,     -0.001471f,
//						0.005015f,     0.002105f,     -0.008842f,     -0.012817f,     -0.003828f,
//						0.003183f,     -0.006564f,     -0.022709f,     -0.022230f,     -0.005689f,
//						-0.000979f,     -0.021868f,     -0.041819f,     -0.030133f,     -0.002845f,
//						-0.006299f,     -0.045843f,     -0.066101f,     -0.028542f,     0.017379f,
//						-0.010767f,     -0.099137f,     -0.116546f,     0.035003f,     0.272081f,
//						0.387823f,     0.272081f,     0.035003f,     -0.116546f,     -0.099137f,
//						-0.010767f,     0.017379f,     -0.028542f,     -0.066101f,     -0.045843f,
//						-0.006299f,     -0.002845f,     -0.030133f,     -0.041819f,     -0.021868f,
//						-0.000979f,     -0.005689f,     -0.022230f,     -0.022709f,     -0.006564f,
//						0.003183f,     -0.003828f,     -0.012817f,     -0.008842f,     0.002105f,
//						0.005015f,     -0.001471f,     -0.005450f,     -0.000788f,     0.005256f,
//						0.004604f,     -0.000129f,     -0.001245f,     0.002328f,     0.004808f,
//						0.002980f,     0.000179f,     0.000345f,     0.002447f,     0.002979f,
//						0.001337f,     0.000024f,     0.000538f,     0.001558f,     0.001382f,
//						0.000326f,     -0.000149f,     0.000359f,     0.000854f,     0.000532f,
//						-0.000165f																	};

static bool filter_signal(float32_t *psrc, float32_t *pdst, uint32_t size){
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


