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

/**
 * @brief Compara 2 features y devuelve estado de la comparacion (pass o fail).
 *
 * Esta funcion compara 2 vectores con features usando la distancia euclideana, calculandola
 * bloque a bloque. Si la distancia de un par de bloque esta por debajo de un determinado threshold,
 * se considera que esos bloques son similares. Si se alcanzan un determinado numero de bloques
 * aceptados, se considera que la voz es reconocida.
 *
 * @param feature_1 String que contiene el nombre del archivo con el feature 1 (terminado en .bin).
 * @param feature_1 String que contiene el nombre del archivo con el feature 2 (terminado en .bin).
 * @return true si la voz fue reconocida, false en caso contrario.
 */
bool compare_features(char *feature_1, char *feature_2){
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

	//Chequeo si pasaron la verificacion la cantidad de bloques requeridos
	is_recognized = (blocks_ok >= ACCEPTED_BLOCK_TH) ? true : false;

	//Devuelvo estado de reconocimiento
	return is_recognized;
}


/**
 * @brief Realiza reconocimiento de voz comparando una voz con un template.
 *
 * Esta funcion captura una señal de voz y la procesa para obtener sus features,
 * que luego seran comparados con un template para decidir si la señal es reconocida
 * o no. Luego se genera la entrada del registro acorde al estado de reconocimiento.
 *
 * @param template_path String con la direccion del template.
 * @param user_name String con el nombre de usuario correspondiente al template.
 * @return true si se reconoce la voz, false en caso contrario.
 */

bool recognize_user_voice(char *template_path, char *user_name){
	//Declaracion de variables
	char entry[ENTRY_STR_SIZE];
	bool res;
	bool is_recognized;

	//Capturo voz
	res = capture_voice_signal(VOICE_PATH);
	if(!res){
		f_unlink(VOICE_PATH);
		return false;
	}

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
