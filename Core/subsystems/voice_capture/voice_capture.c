#include "common_utils.h"
#include "sd_functions.h"
#include "save_and_read_data.h"
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
/*================[Private defines]======================*/
#define ADC_RESOLUTION 4096.0
#define VOLTAGE_REFERENCE 3.3
/*================[Extern variables]=====================*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern volatile bool conv_cplt_flag;

/*================[Public functions]=====================*/

/**
 * @brief Captura señal de voz utilizando el ADC del microcontrolador.
 *
 * @param buffer Puntero uint16_t al buffer donde se guardara la señal capturada.
 * @param size Tamaño del buffer.
 * @return None
 */
static void capture_voice(uint16_t *buffer, uint32_t size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, size);					//Inicio captura de datos con ADC
	while(!conv_cplt_flag);												//Espero a que termine la captura
	conv_cplt_flag = false;
	return;
}


/**
 * @brief Convierte la señal capturada por el ADC a tension.
 *
 * Esta funcion convierte la señal de voz capturada a tension para asi poder interpretarla
 * fisicamente (útil durante el debugging, para encontrar errores en el procesamiento).
 *
 * @param in_buffer Puntero uint16_t al buffer que contiene la voz capturada por el ADC.
 * @param out_buffer Puntero float32_t al vector de salida que contendra las tensiones.
 * @param size Tamaño de ambos vectores de entrada y salida.
 * @return None
 */
static void get_voltage(uint16_t *in_buffer, float32_t *out_buffer, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out_buffer[i] = VOLTAGE_REFERENCE * ((float32_t)in_buffer[i]) / ADC_RESOLUTION;
	}
}

/**
 * @brief Guarda la voz capturada por el ADC en un archivo .bin
 *
 * Esta funcion convierte primero la voz capturada por el ADC en tensiones (float32_t),
 * y luego guarda estos valores en un archivo .bin de a bloques.
 *
 * @param voice_path String con el nombre del archivo terminado en .bin.
 * @param voice_buffer Puntero uint16_t al buffer que contiene la voz capturada por el ADC.
 * @param total_size Tamaño de voice_buffer.
 * @param block_size Tamaño de bloque de procesamiento.
 * @return true si la operacion se llevo a cabo con exito, false en caso contrario.
 */
static bool save_voice(char *voice_path, uint16_t *voice_buffer, uint32_t total_size, uint32_t block_size){
	//Declaro vector de procesamiento
	float32_t voice_block[BLOCK_SIZE] = {0};
	uint32_t num_of_blocks;
	bool res;

	//Verifico que el tamaño total sea multiplo del tamaño del bloque
	if(total_size % block_size != 0){
		return false;
	}

	//Calculo numero de bloques
	num_of_blocks = total_size / block_size;

	//Convierto la voz a float de a bloques
	for(uint8_t i = 0; i < num_of_blocks; i++){
		//Convierto cuentas del adc a valores de tension
		get_voltage(&voice_buffer[i * block_size], voice_block, block_size);

		//Guardo bloque
		res = save_data_on_sd(voice_path, (void *)voice_block, FLOAT_SIZE_BYTES(block_size));
		if(!res){
			send_error(DISPLAY_WRITE_SD_ERROR);							//Envio el error al display
			return false;
		}
	}

	//Si se llego hasta aca, la operacion de guardado fue un exito
	return true;
}

/**
 * @brief Captura y guarda una señal de voz.
 *
 * Esta funcion captura y guarda la señal de voz obtenida por el ADC. La voz
 * sera convertida a tension para poder interpretarla fisicamente.
 *
 * @param voice_path String con el nombre del archivo terminado en .bin.
 * @return true si la operacion se llevo a cabo con exito, false en caso contrario.
 */
bool capture_voice_signal(char *voice_path){
	//Declaracion de variables
	uint16_t voice_buffer[AUDIO_BUFFER_SIZE] = {0};
	bool res;

	//Envio mensaje al display para indicar que comienza reconocimiento de voz
	send_message(DISPLAY_START_SPEECH_REC, BLOCKING);

	//Capturo voz
	capture_voice(voice_buffer, AUDIO_BUFFER_SIZE);

	//Muestro en pantalla que se estan procesando los datos
	send_message(DISPLAY_PROCESSING_DATA, NON_BLOCKING);						//Se envia no bloqueante para seguir procesando datos mientras se muestra el mensaje

	//Almaceno la voz en la memoria SD
	res = save_voice(voice_path, voice_buffer, AUDIO_BUFFER_SIZE, BLOCK_SIZE);
	if(!res){
		//No se pudo capturar correctamente la voz. Retorna false
		return false;
	}

	//Voz capturada con exito
	return true;
}

