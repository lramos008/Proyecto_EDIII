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
	uint32_t last_pos[NUM_OF_VOICES] = {0};
	bool res;

	//Reservo memoria para los vectores de procesamiento
	template = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	current_feature = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	if(template == NULL || current_feature == NULL){
		//Libero memoria si alguno de los 2 arrays fue reservado
		vPortFree(template);
		vPortFree(current_feature);

		//Envio error de reservacion de memoria
		send_error(DISPLAY_MEMORY_ERROR);
		return false;
	}

	arm_fill_f32(0.0f, template, feature_size);
	for(uint8_t i = 0; i < num_of_blocks; i++){
		for(uint8_t j = 0; j < num_of_voices; j++){
			res = read_data_from_sd((char *) feature_filepath[j], (void *) current_feature, FLOAT_SIZE_BYTES(feature_size), &last_pos[j]);
			if(!res){
				//Libero memoria
				vPortFree(template);
				vPortFree(current_feature);

				//Envio error de escritura en sd al display
				send_error(DISPLAY_READ_SD_ERROR);
				return false;
			}

			//Sumo bloque de feature acumulativamente en el bloque template
			arm_add_f32(template, current_feature, template, feature_size);
		}

		//Escalo para obtener el promedio de los features
		arm_scale_f32(template, 1.0f / num_of_voices, template, feature_size);

		//Guardo bloque en el archivo template
		res = save_data_on_sd(CURRENT_TEMPLATE_FILE, (void *) template, FLOAT_SIZE_BYTES(feature_size));
		if(!res){
			//Libero memoria
			vPortFree(template);
			vPortFree(current_feature);
			//Enviar error de escritura de SD al display
			send_error(DISPLAY_WRITE_SD_ERROR);
			return false;
		}

		//Reinicio valores de bloque de template luego de guardarlos en archivo
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

	for(uint8_t i = 0; i < 10; i++){
		feature_path[8] = '0' + i;
		if(!capture_and_store_voice(feature_path)){
			return false;
		}
	}
	return true;
}
#endif
