#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
#include "processing.h"

/*=========================[Private defines]==============================*/
#define	BIN_RATIO_THRESHOLD		0.9f								//Cantidad de bins correctos aceptable
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
 * el mismo tamaño size, que en lo posible debe ser potencia de 2.
 * El filtro fir esta predeterminado por los coeficientes y el numero de taps
 * definidos en este archivo (es modificable).
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param size Tamaño de ambos vectores.
 */
static void fir_filter(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Reservo memoria para el vector de estado que necesita la instancia del filtro FIR
	float32_t *fir_state = pvPortMalloc(FLOAT_SIZE_BYTES(STATE_SIZE));
	if(fir_state == NULL){
		//Manejar error
		return;
	}

	//Inicializo instancia de filtro fir predeterminada
	arm_fir_init_f32(&fir_instance, NUM_TAPS, (float32_t *)filter_taps, fir_state, size);

	//Filtro la señal
	arm_fir_f32(&fir_instance, psrc, pdst, size);

	//Libero memoria utilizada
	vPortFree(fir_state);
	fir_state = NULL;
	return;
}

/**
 * @brief Aplica ventana de hamming a un vector float32_t de tamaño size.
 *
 * Esta funcion multiplica elemento a elemento los valores de un vector float32_t
 * por los coeficientes de Hamming, de forma tal de aplicar ventaneo de Hamming.
 *
 * @param psrc Puntero al vector float32_t
 * @param size Tamaño del vector.
 */

static void hamming_window(float32_t *psrc, uint32_t size){

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
	return;
}

/**
 * @brief Calcula la fft de una señal almacenada en un vector.
 *
 * Esta funcion calcula la fft sobre los elementos de un vector psrc, y
 * coloca la fft resultante en el vector destino pdst. Ambos vectores float32_t
 * deben tener el mismo tamaño size, que en lo posible debe ser potencia de 2.
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param size Tamaño de ambos vectores.
 */
static void calculate_fft(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Inicializo instancia de fft
	arm_rfft_fast_init_f32(&fft_instance, size);

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
 * @param size Tamaño del vector.
 */

static void normalize_array(float32_t *psrc, uint32_t size){
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

/**
 * @brief Calcula valor absoluto de una fft y normaliza.
 *
 * Esta función toma un vector psrc origen que contiene size pares de numeros
 * reales e imaginarios correspondientes a una fft, y procede a calcular el valor
 * absoluto de la fft. Posteriormente se normaliza este resultado y se lo coloca
 * en un vector pdst destino. En este caso size representa la cantidad de pares
 * real/imaginario de la fft en el caso de psrc (el vector tiene 2 * size elementos),
 * y también representa la cantidad de elementos del vector pdst.
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param size Tamaño del vector pdst.
 */
static void get_fft_norm_mag(float32_t *psrc, float32_t *pdst, uint32_t size){
	/*Se tienen en cuenta la cantidad de numeros complejos, no de elementos en p_src*/
	/*Calculo la magnitud de la fft*/
	arm_cmplx_mag_f32(psrc, pdst, size);

	/*Normalizo*/
	normalize_array(pdst, size);
	return;
}


/*=============================[Public functions]====================================*/
/**
 * @brief Procesa un frame de tamaño size correspondiente a una señal de voz.
 *
 * Esta funcion realiza filtrado FIR, ventaneo de hamming y calculo del valor
 * absoluto de la fft, en forma normalizada, de una señal de voz almacenada
 * en un vector psrc. El resultado luego de todos los procesos se almacena en
 * el vector pdst.
 * La relación de tamaños entre psrc y pdst debe ser 2:1, debido a que al calcular
 * la magnitud de la fft se achican las dimensiones a la mitad, y en lo posible
 * ser bases de 2. El parametro size es el tamaño del vector psrc.
 *
 * @param psrc Puntero al vector float32_t de origen.
 * @param pdst Puntero al vector float32_t de destino.
 * @param size Tamaño del vector psrc.
 */
void process_frame(float32_t *psrc, float32_t *pdst, uint32_t size){
	//Reservo memoria para salida filtrada
	float32_t *filtered = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	if(filtered == NULL){
		//Manejar error
		return;
	}

	//Filtro la señal
	fir_filter(psrc, filtered, size);

	//Aplico ventana de hamming al vector
	hamming_window(filtered, size);

	/*Reservo memoria para la salida de FFT y calculo la fft. Libero memoria de salida filtrada*/
	float32_t *fft_output = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	calculate_fft(filtered, fft_output, size);
	vPortFree(filtered);
	filtered = NULL;

	/*Calculo la magnitud de la fft y libero la memoria ocupada por la fft.*/
	get_fft_norm_mag(fft_output, pdst, size / 2);
	vPortFree(fft_output);
	fft_output = NULL;
	return;
}

/**
 * @brief Compara bin a bin y verifica si son similares o no.
 *
 * Esta funcion compara bin a bin los features de 2 vectores. Por feature
 * se entiende a la magnitud normalizada de la fft, que contiene las caracteristicas
 * de la voz. Si una cierta cantidad de bins caen dentro de un rango aceptable, se
 * considera que el frame analizado pasa la prueba de comparación.
 *
 * @param template Puntero al template con los features guardados de antemano.
 * @param target Puntero al vector con los features obtenidos de la voz actual.
 * @param size Tamaño de los vectores template y target.
 * @return comp_state Estado de la comparacion de features.
 */
bool compare_features(float32_t *template, float32_t *target, uint32_t size){
	uint32_t accepted_bins = 0;										//Bins dentro del threshold
	float32_t accepted_bins_ratio;									//Proporcion de bins aceptados con respecto al total
	float32_t *diff = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	float32_t *abs_diff = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	bool comp_state;												//Estado de comparacion de los features

	//Resto bin a bin los features y calculo el valor absoluto
	arm_sub_f32(template, target, diff, size);
	arm_abs_f32(diff, abs_diff, size);

	//Cuento cuantos bins estan por debajo del threshold
	for(uint32_t i = 0; i < size; i++){
		if(abs_diff[i] < DIF_THRESHOLD){
			accepted_bins++;
		}
	}

	/*Libero la memoria utilizada*/
	vPortFree(diff);
	vPortFree(abs_diff);

	//Calculo el ratio de bins aceptados
	accepted_bins_ratio = ((float32_t) accepted_bins) / size;

	//Verifico si el ratio de bins aceptados es mayor al threshold BIN_RATIO_THRESHOLD
	comp_state = (accepted_bins_ratio >= BIN_RATIO_THRESHOLD) ? true : false;
	return comp_state;
}




