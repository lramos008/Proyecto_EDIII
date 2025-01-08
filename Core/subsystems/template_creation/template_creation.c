#include "common_utils.h"
#include "voice_capture.h"
#include "feature_extraction.h"
#include "sd_functions.h"
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"

#define CURRENT_VOICE_FILE "current_voice.bin"
#define CURRENT_TEMPLATE_FILE "current_template.bin"
#define NUM_OF_VOICES 5
#define FEATURE_STR_SIZE 20


const char feature_filepath[NUM_OF_VOICES][FEATURE_STR_SIZE] = {"feature_1.bin",
															    "feature_2.bin",
															    "feature_3.bin",
															    "feature_4.bin",
															    "feature_5.bin"	};

static bool capture_and_extract_features(uint8_t num_of_blocks, uint8_t num_of_voices, uint32_t total_size, uint32_t block_size){
	//Capturo 5 voces
	for(uint8_t i = 0; i < num_of_voices; i++){
		if(!capture_and_store_voice(CURRENT_VOICE_FILE) ||
		   !extract_features_from_file(CURRENT_VOICE_FILE, (char *) feature_filepath[i], total_size, block_size, OVERLAP_RATIO)){
			//Mostrar error de captura

			//Elimino voz actual capturada
			f_unlink(CURRENT_VOICE_FILE);

			//Elimino todos los archivos de features creados hasta el momento
			for(uint8_t j = 0; j <= i; j++){
				f_unlink(feature_filepath[j]);
			}
			return false;
		}
		f_unlink(CURRENT_VOICE_FILE);
	}
	return true;
}

static bool process_features(uint8_t num_of_blocks, uint8_t num_of_voices, uint32_t feature_size){
	float32_t *template = NULL;
	float32_t *current_feature = NULL;
	FRESULT res;

	//Reservo memoria para los vectores de procesamiento
	template = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	current_feature = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	if(template == NULL || current_feature == NULL){
		//Enviar error al reservar memoria

		vPortFree(template);
		vPortFree(current_feature);
		return false;
	}

	for(uint8_t i = 0; i < num_of_blocks; i++){
		for(uint8_t j = 0; j < num_of_voices; j++){
			res = read_buffer_from_sd((char *) feature_filepath[j], current_feature, feature_size, i * feature_size);
			if(res != FR_OK){
				//Enviar error de lectura

				vPortFree(template);
				vPortFree(current_feature);
				return false;
			}

			//Sumo bloque de feature acumulativamente en el bloque template
			arm_add_f32(template, current_feature, template, feature_size);
		}

		//Escalo para obtener el promedio de los features
		arm_scale_f32(template, 1.0f / num_of_voices, template, feature_size);

		//Guardo bloque en el archivo template
		res = save_buffer_on_sd(CURRENT_TEMPLATE_FILE, template, feature_size);
		if(res != FR_OK){
			//Enviar error de escritura

			vPortFree(template);
			vPortFree(current_feature);
			return false;
		}
		arm_fill_f32(0.0f, template, feature_size);
	}

	//Borro archivos de features creados
	for(uint8_t i = 0; i < num_of_voices; i++){
		f_unlink(feature_filepath[i]);
	}

	//Libero memoria
	vPortFree(template);
	vPortFree(current_feature);
	return true;
}

bool generate_template(void){
	//Declaracion de variables
	bool is_generated;
	uint32_t num_of_blocks = (uint32_t)((AUDIO_BUFFER_SIZE / BLOCK_SIZE) / OVERLAP_RATIO);

	//Capturo voces y genero los features asociados
	if(!capture_and_extract_features(num_of_blocks, NUM_OF_VOICES, AUDIO_BUFFER_SIZE, BLOCK_SIZE)){
		//Error de captura

		return false;
	}

	//Proceso features para generar el template
	is_generated = process_features(num_of_blocks, NUM_OF_VOICES, FEATURE_SIZE);
	return is_generated;
}

