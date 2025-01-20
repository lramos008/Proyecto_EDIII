#include "sd_functions.h"
#include "feature_extraction.h"
#include "save_and_read_data.h"
#define STATE_SIZE 	(NUM_TAPS + BLOCK_SIZE - 1)

bool filter_signal(char *input_file, char *output_file){
	//Declaro vectores de procesamiento y variables a utilizar
	arm_fir_instance_f32 fir_instance;
	float32_t voice_block[BLOCK_SIZE] = {0};
	float32_t filtered_block[BLOCK_SIZE] = {0};
	float32_t state_vector[STATE_SIZE] = {0};
	uint32_t last_pos = 0;
	uint32_t file_size;
	uint32_t num_of_blocks;
	uint32_t remaining_samples;
	bool res;

	//Inicializo filtro fir
	initialize_filter(&fir_instance, state_vector, BLOCK_SIZE);

	//Calculo la cantidad de bloques
	file_size = get_file_size(input_file);
	num_of_blocks = file_size / (FLOAT_SIZE_BYTES(BLOCK_SIZE));
	remaining_samples = (file_size / sizeof(float32_t)) % BLOCK_SIZE;

	for(uint32_t i = 0; i < num_of_blocks; i++){
		//Leo bloque teniendo en cuenta el solapamiento
		res = read_data_from_sd(input_file, (void *) voice_block, FLOAT_SIZE_BYTES(BLOCK_SIZE), &last_pos);
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Filtro el bloque
		filter_block(&fir_instance, voice_block, filtered_block, BLOCK_SIZE);

		//Guardo bloque filtrado en archivo
		res = save_data_on_sd(output_file, (void *) filtered_block, FLOAT_SIZE_BYTES(BLOCK_SIZE));
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}
	}

	//Verifico si hay que procesar muestras restantes
	if(remaining_samples > 0){
		//Hago zero padding en el vector de voz para completar el block size
		arm_fill_f32(0.0f, voice_block, BLOCK_SIZE);

		//Leo muestras restantes
		res = read_data_from_sd(input_file, (void *) voice_block, FLOAT_SIZE_BYTES(remaining_samples), &last_pos);
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Filtro muestras restantes
		filter_block(&fir_instance, voice_block, filtered_block, BLOCK_SIZE);

		//Guardo los datos filtrados restantes
		res = save_data_on_sd(output_file, (void *) filtered_block, FLOAT_SIZE_BYTES(remaining_samples));
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}
	}

	//Devuelvo false si todas las operaciones de lectura / escritura fueron correctas. Caso contrario false.
	return true;
}

bool get_fft_feature(char *input_file, char *output_file){
	//Declaro vectores de procesamiento y variables importantes
	arm_rfft_fast_instance_f32 rfft_instance;
	float32_t voice_block[BLOCK_SIZE] = {0};
	float32_t fft_block[BLOCK_SIZE] = {0};
	float32_t feature_block[FEATURE_SIZE] = {0};
	uint32_t step = BLOCK_SIZE - OVERLAP;
	uint32_t last_pos;
	uint32_t file_size;
	uint32_t total_samples;
	uint32_t num_of_blocks;
	bool res;

	//Calculo cantidad de bloques (se espera que sea multiplo de block size)
	file_size = get_file_size(input_file);
	total_samples = file_size / sizeof(float32_t);
	num_of_blocks = (total_samples - OVERLAP) / OVERLAP;

	//Inicializo la fft
	res = initialize_fft(&rfft_instance, BLOCK_SIZE);
	if(!res){
		send_error(DISPLAY_MEMORY_ERROR);											//Envio error al display
		return false;																//No se pudo completar correctamente la operacion
	}

	//Leo de a bloques, con solapamiento de 50%, y obtengo los features
	for(uint32_t start = 0; start <= total_samples - BLOCK_SIZE; start += step){
		//Leo bloque desde archivo
		last_pos = FLOAT_SIZE_BYTES(start);
		res = read_data_from_sd(input_file, (void *) voice_block, FLOAT_SIZE_BYTES(BLOCK_SIZE), &last_pos);
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Aplico ventana de hamming al bloque
		apply_hamming_windowing(voice_block, BLOCK_SIZE);

		//Calculo la fft
		calculate_rfft(&rfft_instance, voice_block, fft_block);

		//Calculo la magnitud de la fft
		calculate_magnitude(fft_block, feature_block, FEATURE_SIZE);				//El tamaño es la cantidad de nros complejos en el vector de entrada

		//Normalizo el array
		normalize_array(feature_block, FEATURE_SIZE);

		//Guardo bloque procesado en la memoria SD
		res = save_data_on_sd(output_file, (void *) feature_block, FLOAT_SIZE_BYTES(FEATURE_SIZE));
		if(!res){
			//Manejo error de lectura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}
	}

	//Features extraidas exitosamente
	return true;
}

