#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
#include "processing.h"

/*=========================[Private defines]==============================*/
#define	ACCEPTED_BINS_THRESHOLD 0.9f								//Cantidad de bins correctos aceptable
#define DIF_THRESHOLD 			0.1f								//Diferencia aceptable entre bins
#define NUM_TAPS 				50
#define STATE_SIZE 				(NUM_TAPS + FRAME_SIZE - 1)

/*=========================[Private variables]============================*/
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


/*=========================[Private functions]==============================*/
/**
 * @brief Aplica filtro fir predeterminado (ver mas arriba) a un vector.
 *
 * Esta funcion aplica filtro fir a los elementos del vector psrc, y coloca el
 * resultado en el vector destino pdst. Ambos vectores float32_t deben tener
 * el mismo tamaño block_size, que en lo posible debe ser potencia de 2.
 * El filtro fir esta predeterminado por los coeficientes y el numero de taps
 * definidos en este archivo (es modificable).
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param block_size Tamaño de ambos vectores.
 */
static void fir_filter(float32_t *psrc, float32_t *pdst, size_t block_size){
	//Reservo memoria para el vector de estado que necesita la instancia del filtro FIR
	float32_t *fir_state = pvPortMalloc(FLOAT_SIZE_BYTES(STATE_SIZE));
	if(fir_state == NULL){
		//Manejar error
		return;
	}

	//Inicializo instancia de filtro fir predeterminada
	arm_fir_init_f32(&fir_instance, NUM_TAPS, (float32_t *)filter_taps, fir_state, block_size);

	//Filtro la señal
	arm_fir_f32(&fir_instance, psrc, pdst, block_size);

	//Libero memoria utilizada
	vPortFree(fir_state);
	fir_state = NULL;
	return;
}

/**
 * @brief Aplica ventana de hamming a un vector float32_t de tamaño block_size
 *
 * Esta funcion multiplica elemento a elemento los valores de un vector float32_t
 * por los coeficientes de Hamming, de forma tal de aplicar ventaneo de Hamming.
 *
 * @param psrc Puntero al vector float32_t
 * @param block_size Tamaño del vector.
 */

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

/**
 * @brief Calcula la fft de una señal almacenada en un vector.
 *
 * Esta funcion calcula la fft sobre los elementos de un vector psrc, y
 * coloca la fft resultante en el vector destino pdst. Ambos vectores float32_t
 * deben tener el mismo tamaño block_size, que en lo posible debe ser potencia de 2.
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param block_size Tamaño de ambos vectores.
 */
static void calculate_fft(float32_t *psrc, float32_t *pdst, size_t block_size){
	//Inicializo instancia de fft
	arm_rfft_fast_init_f32(&fft_instance, block_size);

	/*Calculo la fft*/
	arm_rfft_fast_f32(&fft_instance, psrc, pdst, 0);					//El cero indica que se hace transformada, no antitransformada
	return;
}

/**
 * @brief Normaliza los valores de un vector float32_t.
 *
 * Esta funcion normaliza los valores de un vector float32_t.
 * Luego de aplicada la función, todos los valores del vector
 * pasaran a encontrarse en el rango entre 0 y 1.
 *
 * @param psrc Puntero al vector float32_t.
 * @param block_size Tamaño del vector.
 */

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

/**
 * @brief Calcula valor absoluto de una fft y normaliza.
 *
 * Esta función toma un vector psrc origen que contiene una fft,
 * y procede a hallar su valor absoluto. Posteriormente a eso se
 * lo normaliza, y se coloca el resultado en un vector pdst destino.
 * Ambos vectores deben ser float32_t, con igual tamaño block_size.
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param block_size Tamaño de ambos vectores.
 */
static void get_fft_norm_mag(float32_t *psrc, float32_t *pdst, size_t block_size){
	/*Se tienen en cuenta la cantidad de numeros complejos, no de elementos en p_src*/
	/*Calculo la magnitud de la fft*/
	arm_cmplx_mag_f32(psrc, pdst, block_size);

	/*Normalizo*/
	normalize_array(pdst, block_size);
	return;
}



