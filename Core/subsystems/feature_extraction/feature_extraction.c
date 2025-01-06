#include "common_utils.h"
#include "sd_functions.h"


bool extract_features_from_file(char *target_file, char *feature_file){
	float32_t *voice_block = NULL;
	float32_t *feature_block = NULL;
	uint32_t overlap = (uint32_t)(BLOCK_SIZE * OVERLAP_RATIO);
	uint32_t stride = BLOCK_SIZE - overlap;

	//Reservo memoria para los vectores de procesamiento
	voice_block = pvPortMalloc(FLOAT_SIZE_BYTES(BLOCK_SIZE));
	feature_block = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(voice_block == NULL || feature_block == NULL){
		vPortFree(voice_block);
		vPortFree(feature_block);
		return false;
	}

	//Leo de a bloques, con solapamiento de 50%, y obtengo los features
	for(uint32_t start = 0; start + BLOCK_SIZE <= AUDIO_BUFFER_SIZE; start += stride){
		read_buffer_from_sd(target_file, voice_block, BLOCK_SIZE, start);
		if(!process_block(voice_block, feature_block, BLOCK_SIZE)){
			vPortFree(voice_block);
			vPortFree(feature_block);
			return false;
		}

		//Guardo feature en la SD
		save_buffer_on_sd(feature_file, feature_block, FEATURE_SIZE);
	}

	//Elimino archivo de voz creado en la SD
	f_unlink(target_file);

	//Libero memoria utilizada
	vPortFree(voice_block);
	vPortFree(feature_block);
	return true;							//Extraccion de features exitosa
}
