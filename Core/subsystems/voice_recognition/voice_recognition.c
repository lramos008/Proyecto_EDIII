#include "common_utils.h"
#include "voice_capture.h"
#include "feature_extraction.h"
#include "compare_features.h"
#include "file_handling.h"

#define CURRENT_VOICE_FILEPATH    "current_voice.bin"
#define CURRENT_FEATURE_FILEPATH  "current_feature.bin"
#define REGISTER_PATH 	"registro.txt"

bool check_voice(char *template_path, char *feature_path){
	bool compare_res = false;
	bool is_recognized = false;
	uint8_t frame_counter = 0;
	float32_t *template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	float32_t *extracted_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(template == NULL || extracted_feature == NULL){
		vPortFree(template);
		vPortFree(extracted_feature);
		return false;
	}

	for(uint8_t i = 0; i < (NUM_OF_BLOCKS / OVERLAP_RATIO); i++){
		//Leo cada bloque del archivo, y comparo bin a bin
		read_buffer_from_sd(template_path, template, FEATURE_SIZE, i * FEATURE_SIZE);
		read_buffer_from_sd(feature_path, extracted_feature, FEATURE_SIZE, i * FEATURE_SIZE);
		compare_res = compare_features(template, extracted_feature, FEATURE_SIZE);
		if(compare_res == false){
			break;
		}
		frame_counter++;																	//Aumento el conteo de bloques correctos
	}

	//Libero memoria
	vPortFree(template);
	vPortFree(extracted_feature);

	//Compruebo si hay NUM_OF_BLOCKS bloques correctos
	is_recognized = (frame_counter < NUM_OF_FRAMES) ? false : true;
	return is_recognized;																	//Devuelvo estado del reconocimiento
}

bool recognize_user_voice(char *template_path, char *user_name){
	//Declaracion de variables
	char entry[ENTRY_STR_SIZE];
	//Capturo y guardo la voz en la memoria SD
	if(!capture_and_store_voice(CURRENT_VOICE_FILE) || !extract_features_from_file(CURRENT_VOICE_FILE, CURRENT_FEATURE_FILEPATH)){
		return false;
	}


	//Compruebo si la voz capturada es similar al template
	if(check_voice(template_path, CURRENT_FEATURE_FILEPATH)){
		build_entry_message(entry, user_name, "Concedido\n");
		write_entry(REGISTER_PATH, entry);
		f_unlink(CURRENT_FEATURE_FILEPATH);										//Borro archivo de feature capturado recientemente
		return true;
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
		write_entry(REGISTER_PATH, entry);
		f_unlink(CURRENT_FEATURE_FILEPATH);
		return false;
	}
}
