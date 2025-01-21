#include "common_utils.h"
#include "voice_capture.h"
#include "feature_extraction.h"
#include "sd_functions.h"
#include "save_and_read_data.h"
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

//#define V1
#ifdef V1
bool generate_template(void){
	//Declaracion de variables
	bool is_generated;
	uint32_t num_of_blocks = (uint32_t)(((AUDIO_BUFFER_SIZE / BLOCK_SIZE) / OVERLAP_RATIO) - 1);

	//Capturo voces y genero los features asociados
	if(!capture_and_extract_features(num_of_blocks, NUM_OF_VOICES, AUDIO_BUFFER_SIZE, BLOCK_SIZE)){
		//Error de captura

		return false;
	}


	//Proceso features para generar el template
	is_generated = process_features(num_of_blocks, NUM_OF_VOICES, FEATURE_SIZE);

	return is_generated;
}
#else
bool generate_template(void){
	char feature_path[15] = "feature_x.bin";
	bool res;
	for(uint8_t i = 0; i < NUM_OF_TEMPLATE_SAMPLES; i++){
		feature_path[8] = '0' + i;											//Genero el path del feature x

		//Capturo señal de voz
		res = capture_voice_signal(feature_path);
		if(!res){
			//De haber un error, borro los archivos creados
			for(uint8_t j = 0; j <= i; j++){
				feature_path[8] = '0' + j;
				f_unlink(feature_path);
			}
			return false;
		}
	}

	//Si se creo el template correctamente devuelve true
	return true;
}
#endif
