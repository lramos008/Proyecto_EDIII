#include "sd_functions.h"
#include "feature_extraction.h"
#include "block_processing.h"
#include "save_and_read_data.h"
#define STATE_SIZE 	(NUM_TAPS + BLOCK_SIZE - 1)

bool extract_features_from_file(char *target_file, char *feature_file, uint32_t total_size,  uint32_t block_size, float32_t overlap_ratio){
	//Declaracion de variables
	float32_t *voice_block = NULL;
	float32_t *feature_block = NULL;
	uint32_t overlap = (uint32_t)(block_size * overlap_ratio);
	uint32_t stride = block_size - overlap;
	uint32_t last_pos = 0;
	//Reservo memoria para los vectores de procesamiento
	voice_block = pvPortMalloc(FLOAT_SIZE_BYTES(block_size));
	feature_block = pvPortMalloc(FLOAT_SIZE_BYTES(block_size / 2));
	if(voice_block == NULL || feature_block == NULL){
		//Libero memoria si se reservo alguno de los bloques
		vPortFree(voice_block);
		vPortFree(feature_block);

		//Manejo error de reserva de memoria
		send_error(DISPLAY_MEMORY_ERROR);
		return false;
	}

	//Leo de a bloques, con solapamiento de 50%, y obtengo los features
	for(uint32_t start = 0; start + block_size <= total_size; start += stride){
		//Leo bloque de voz desde archivo
		if(!read_data_from_sd(target_file, (void *) voice_block, FLOAT_SIZE_BYTES(block_size), &last_pos)){
			//Libero memoria
			vPortFree(voice_block);
			vPortFree(feature_block);

			//Borro archivo de features
			f_unlink(feature_file);

			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Proceso bloque para obtener los features
		if(!process_block(voice_block, feature_block, block_size)){
			vPortFree(voice_block);
			vPortFree(feature_block);
			f_unlink(feature_file);
			//Manejar error

			return false;
		}


		//Guardo feature en la SD
		if(!save_data_on_sd(feature_file, (void *) feature_block, FLOAT_SIZE_BYTES(block_size / 2))){
			//Libero memoria y borro archivo de features
			vPortFree(voice_block);
			vPortFree(feature_block);
			f_unlink(feature_file);

			//Manejo error de escritura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}

	}

	//Libero memoria utilizada
	vPortFree(voice_block);
	vPortFree(feature_block);
	return true;							//Extraccion de features exitosa
}

bool filter_signal(char *input_file, char *output_file, uint32_t block_size){
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
	initialize_filter(&fir_instance, state_vector, block_size);

	//Calculo la cantidad de bloques
	file_size = get_file_size(input_file);
	num_of_blocks = file_size / (FLOAT_SIZE_BYTES(block_size));
	remaining_samples = (file_size / sizeof(float32_t)) % block_size;

	for(uint32_t i = 0; i < num_of_blocks; i++){
		//Leo bloque teniendo en cuenta el solapamiento
		res = read_data_from_sd(input_file, (void *) voice_block, FLOAT_SIZE_BYTES(block_size), &last_pos);
		if(!res){
			//Borro archivo de features y de voz
			f_unlink(input_file);
			f_unlink(output_file);

			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Filtro el bloque
		filter_block(&fir_instance, voice_block, filtered_block, block_size);

		//Guardo bloque filtrado en archivo
		res = save_data_on_sd(output_file, (void *) filtered_block, FLOAT_SIZE_BYTES(block_size));
		if(!res){
			//Borro archivo de features
			f_unlink(input_file);
			f_unlink(output_file);

			//Manejo error de lectura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}
	}

	//Verifico si hay que procesar muestras restantes
	if(remaining_samples > 0){
		//Hago zero padding en el vector de voz para completar el block size
		arm_fill_f32(0.0f, voice_block, block_size);

		//Leo muestras restantes
		res = read_data_from_sd(input_file, (void *) voice_block, FLOAT_SIZE_BYTES(remaining_samples), &last_pos);
		if(!res){
			//Borro archivo de features y de voz
			f_unlink(input_file);
			f_unlink(output_file);

			//Manejo error de lectura en SD
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Filtro muestras restantes
		filter_block(&fir_instance, voice_block, filtered_block, block_size);

		//Guardo los datos filtrados restantes
		res = save_data_on_sd(output_file, (void *) filtered_block, FLOAT_SIZE_BYTES(remaining_samples));
		if(!res){
			//Borro archivo de features
			f_unlink(input_file);
			f_unlink(output_file);

			//Manejo error de lectura en SD
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}
	}

	//Borro el archivo de voz original y me quedo con el filtrado
	f_unlink(input_file);

	//Devuelvo false si todas las operaciones de lectura / escritura fueron correctas. Caso contrario false.
	return true;
}

bool get_fft_feature(char *input_file, char *output_file, uint32_t block_size, uint32_t overlap){
	//Declaro vectores de procesamiento y variables importantes
	float32_t voice_block[BLOCK_SIZE] = {0};
	float32_t feature_block[FEATURE_SIZE] = {0};
	uint32_t step = block_size - overlap;
	uint32_t last_pos = 0;
	uint32_t file_size;
	uint32_t total_samples;
	uint32_t num_of_blocks;
	bool res;

	//Calculo cantidad de bloques (se espera que sea multiplo de block size)
	file_size = get_file_size(input_file);
	total_samples = file_size / sizeof(float32_t);
	num_of_blocks = (total_samples - overlap) / overlap;

	//Leo de a bloques, con solapamiento de 50%, y obtengo los features
	for(uint32_t start = 0; start + block_size <= total_size; start += stride){

	}


}

