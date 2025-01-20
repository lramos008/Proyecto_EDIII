#include "common_utils.h"
#include "euclidean_distance.h"
#include "voice_capture.h"
#include "feature_extraction.h"
#include "file_handling.h"
#include "save_and_read_data.h"
#include "user_and_entry.h"
#include "sd_functions.h"

#define VOICE_PATH    "captured_voice.bin"
#define FILTERED_PATH "filtered_signal.bin"
#define FEATURE_PATH  "current_feature.bin"
#define REGISTER_PATH 	"registro.txt"

static bool compare_features(char *feature_1, char *feature_2){
	//Declaro vectores de procesamiento
	float32_t feature1_block[FEATURE_SIZE] = {0};
	float32_t feature2_block[FEATURE_SIZE] = {0};
	float32_t distance = 0;
	uint32_t blocks_ok = 0;
	uint32_t last_pos[2] = {0};
	bool is_recognized;

	for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
		//Leo cada bloque del archivo, y comparo bin a bin
		if(!read_data_from_sd(feature_1, (void *) feature1_block, FLOAT_SIZE_BYTES(FEATURE_SIZE), &last_pos[0]) ||
		   !read_data_from_sd(feature_2, (void *) feature2_block, FLOAT_SIZE_BYTES(FEATURE_SIZE), &last_pos[1])){
			send_error(DISPLAY_READ_SD_ERROR);
			return false;
		}

		//Calculo la distancia euclideana
		distance = euclidean_distance(feature1_block, feature2_block);

		//Verifico si es menor al threshold
		if(distance <= DISTANCE_THRESHOLD){
			blocks_ok++;
		}
	}




}




bool recognize_user_voice(char *template_path, char *user_name){
	//Declaracion de variables
	char entry[ENTRY_STR_SIZE];
	bool res;
	bool is_recognized;

	//Capturo voz
	capture_voice_signal(VOICE_PATH);

	//Filtro la señal
	res = filter_signal(VOICE_PATH, FILTERED_PATH);
	if(!res){
		//Borro archivo de voz y de señal filtrada
		f_unlink(VOICE_PATH);
		f_unlink(FILTERED_PATH);
		return false;
	}

	//Obtengo features
	res = get_fft_feature(FILTERED_PATH, FEATURE_PATH);
	if(!res){
		//Borro archivo de señal filtrada y de feature
		f_unlink(VOICE_PATH);
		f_unlink(FILTERED_PATH);
		f_unlink(FEATURE_PATH);
		return false;
	}

	//Comparo features con el template
	is_recognized = compare_features(FEATURE_PATH, template_path);

	if(is_recognized){
		build_entry_message(entry, user_name, "Concedido");
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
	}

	//Escribo entrada de registro
	write_entry(REGISTER_PATH, entry);

	//Elimino los archivos creados en el reconocimiento de voz
	f_unlink(VOICE_PATH);
	f_unlink(FILTERED_PATH);
	f_unlink(FEATURE_PATH);

	//Devuelvo estado de reconocimiento
	return is_recognized;
}
