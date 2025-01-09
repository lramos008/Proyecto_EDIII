#include "common_utils.h"
#include "voice_capture.h"
#include "feature_extraction.h"
#include "compare_features.h"
#include "file_handling.h"
#include "sd_functions.h"

#define CURRENT_VOICE_FILEPATH    "current_voice.bin"
#define CURRENT_FEATURE_FILEPATH  "current_feature.bin"
#define REGISTER_PATH 	"registro.txt"


static bool check_voice(char *template_path, char *feature_path, uint32_t feature_size){
	bool compare_res = false;
	bool is_recognized = false;
	uint32_t block_counter = 0;
	uint32_t num_of_blocks = (AUDIO_BUFFER_SIZE / BLOCK_SIZE) * (uint32_t) (1 / OVERLAP_RATIO);
	float32_t block_ratio;
	float32_t *template = NULL;
	float32_t *extracted_feature = NULL;

	//Reservo memoria para los arrays de procesamiento
	template = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	extracted_feature = pvPortMalloc(FLOAT_SIZE_BYTES(feature_size));
	if(template == NULL || extracted_feature == NULL){
		//No se pudo reservar memoria. Libero ambos arrays.
		vPortFree(template);
		vPortFree(extracted_feature);

		//Envio mensaje de error al display
		send_error(DISPLAY_MEMORY_ERROR);
		return false;
	}


	for(uint32_t i = 0; i < num_of_blocks; i++){
		//Leo cada bloque del archivo, y comparo bin a bin
		if(read_buffer_from_sd(template_path, template, feature_size, i * feature_size) != FR_OK ||
		   read_buffer_from_sd(feature_path, extracted_feature, feature_size, i * feature_size) != FR_OK){
			//Libero memoria utilizada
			vPortFree(template);
			vPortFree(extracted_feature);

			//Mostrar pantalla de error
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		compare_res = compare_features(template, extracted_feature, feature_size);
		if(compare_res){
			block_counter++;														//Aumento el conteo de bloques correctos
		}
	}

	//Libero memoria
	vPortFree(template);
	vPortFree(extracted_feature);

	//Compruebo si hay NUM_OF_BLOCKS bloques correctos
	block_ratio = ((float32_t) block_counter) / ((float32_t) num_of_blocks);				//Calculo el ratio de bloques aceptados
	is_recognized = (block_ratio < ACCEPTED_BLOCK_RATIO) ? false : true;
	return is_recognized;																	//Devuelvo estado del reconocimiento
}



bool recognize_user_voice(char *template_path, char *user_name){
	//Declaracion de variables
	char entry[ENTRY_STR_SIZE];
	bool is_recognized;

	//Capturo y guardo la voz en la memoria SD
	if(!capture_and_store_voice(CURRENT_VOICE_FILEPATH) ||
	   !extract_features_from_file(CURRENT_VOICE_FILEPATH, CURRENT_FEATURE_FILEPATH, AUDIO_BUFFER_SIZE, BLOCK_SIZE, OVERLAP_RATIO)){
		//Mostrar error en captura

		return false;
	}

	//Elimino la voz creada y me quedo solo con los features
	f_unlink(CURRENT_VOICE_FILEPATH);

	//Compruebo si la voz capturada es similar al template
	if(check_voice(template_path, CURRENT_FEATURE_FILEPATH, FEATURE_SIZE)){
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
