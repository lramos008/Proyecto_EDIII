#include "sd_functions.h"
#include "feature_extraction.h"
#include "block_processing.h"

bool extract_features_from_file(char *target_file, char *feature_file, uint32_t total_size,  uint32_t block_size, float32_t overlap_ratio){
	//Declaracion de variables
	float32_t *voice_block = NULL;
	float32_t *feature_block = NULL;
	uint32_t overlap = (uint32_t)(block_size * overlap_ratio);
	uint32_t stride = block_size - overlap;

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
		if(read_buffer_from_sd(target_file, voice_block, block_size, start) != FR_OK){
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
		if(save_buffer_on_sd(feature_file, feature_block, block_size / 2) != FR_OK){
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
