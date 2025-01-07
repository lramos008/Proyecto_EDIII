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
		vPortFree(voice_block);
		vPortFree(feature_block);
		return false;
	}

	//Leo de a bloques, con solapamiento de 50%, y obtengo los features
	for(uint32_t start = 0; start + block_size <= total_size; start += stride){
		read_buffer_from_sd(target_file, voice_block, block_size, start);
		if(!process_block(voice_block, feature_block, block_size)){
			vPortFree(voice_block);
			vPortFree(feature_block);
			f_unlink(feature_file);
			return false;
		}

		//Guardo feature en la SD
		save_buffer_on_sd(feature_file, feature_block, block_size / 2);
	}

	//Libero memoria utilizada
	vPortFree(voice_block);
	vPortFree(feature_block);
	return true;							//Extraccion de features exitosa
}
