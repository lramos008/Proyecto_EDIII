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
	uint32_t block_counter = 0;
	float32_t block_ratio;
	float32_t *template = NULL;
	float32_t *extracted_feature = NULL;

	//Reservo memoria para los arrays de procesamiento
	template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	extracted_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(template == NULL || extracted_feature == NULL){
		//No se pudo reservar memoria. Libero ambos arrays.
		vPortFree(template);
		vPortFree(extracted_feature);
		return false;
	}

	for(uint32_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Leo cada bloque del archivo, y comparo bin a bin
		read_buffer_from_sd(template_path, template, FEATURE_SIZE, i * FEATURE_SIZE);
		read_buffer_from_sd(feature_path, extracted_feature, FEATURE_SIZE, i * FEATURE_SIZE);
		compare_res = compare_features(template, extracted_feature, FEATURE_SIZE);
		if(compare_res == false){
			break;
		}
		block_counter++;																	//Aumento el conteo de bloques correctos
	}

	//Libero memoria
	vPortFree(template);
	vPortFree(extracted_feature);

	//Compruebo si hay NUM_OF_BLOCKS bloques correctos
	block_ratio ((float32_t) block_counter) / NUM_OF_BLOCKS;								//Calculo el ratio de bloques aceptados
	is_recognized = (block_ratio < ACCEPTED_BLOCK_RATIO) ? false : true;
	return is_recognized;																	//Devuelvo estado del reconocimiento
}

bool recognize_user_voice(char *template_path, char *user_name){
	//Declaracion de variables
	char entry[ENTRY_STR_SIZE];
	bool is_recognized;

	//Capturo y guardo la voz en la memoria SD
	if(!capture_and_store_voice(CURRENT_VOICE_FILE) || !extract_features_from_file(CURRENT_VOICE_FILE, CURRENT_FEATURE_FILEPATH)){
		return false;
	}


	//Compruebo si la voz capturada es similar al template
	if(check_voice(template_path, CURRENT_FEATURE_FILEPATH)){
		build_entry_message(entry, user_name, "Concedido");
		is_recognized = true;
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
		is_recognized = false;
	}
	write_entry(REGISTER_PATH, entry);
	f_unlink(CURRENT_FEATURE_FILEPATH);
	return is_recognized;
}
