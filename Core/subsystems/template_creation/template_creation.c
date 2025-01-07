#include "common_utils.h"
#include "voice_capture.h"

#define CURRENT_VOICE_FILE "current_voice.bin"
#define CURRENT_TEMPLATE_FILE "current_template.bin"
#define NUM_OF_VOICES 5
#define FEATURE_STR_SIZE 20

static bool process_feature_blocks(char *filenames,
					  uint8_t num_of_blocks,
					  uint8_t num_of_voices,
					  float32_t *template,
					  float32_t *current_feature,
					  uint32_t feature_size,
					  uint8_t block_index){
	//Declaracion de variables}
	for(uint8_t i = 0; i < num_of_voices; i++){
		read_buffer_from_sd(filenames[i], current_feature, feature_size, block_index * feature_size);
		arm_add_f32(template, current_feature, template, feature_size);
	}

	//Escalo para obtener el promedio
	arm_scale_f32(template, 1.0f / num_of_voices, template, feature_size);
	return true;
}

static bool process_features(char *filenames,
					  uint8_t num_of_blocks,
					  uint8_t num_of_voices,
					  float32_t *template,
					  float32_t *current_feature,
					  uint32_t feature_size){
	//Declaro variables
	arm_fill_f32(0.0f, template, feature_size);

	//Proceso los bloques de cada feature para armar el template
	for(uint8_t i = 0; i < num_of_blocks; i++){
		if(!process_feature_blocks(filenames, num_of_blocks, num_of_voices, template, current_feature, feature_size)){
			return false;
		}

		//Guardo bloque de template en archivo
		save_buffer_on_sd(CURRENT_TEMPLATE_FILE, template, feature_size);
	}

	//Procesado exitoso
	return true;
}


bool generate_template(void){
	//Declaracion de variables
	display_message_t message;
	char feature_filepath[NUM_OF_VOICES][FEATURE_STR_SIZE] = {"feature_1.bin",
															  "feature_2.bin",
															  "feature_3.bin",
															  "feature_4.bin",
															  "feature_5.bin"	};
	float32_t *template = NULL;
	float32_t *current_feature = NULL;
	uint32_t num_of_blocks = (uint32_t)((AUDIO_BUFFER_SIZE / BLOCK_SIZE) / OVERLAP_RATIO);

	//Capturo 5 voces
	for(uint8_t i = 0; i < NUM_OF_VOICES; i++){
		if(!capture_and_store_voice(CURRENT_VOICE_FILE) ||
		   !extract_features_from_file(CURRENT_VOICE_FILE, feature_filepath[i], AUDIO_BUFFER_SIZE, BLOCK_SIZE, OVERLAP_RATIO)){
			return false;
		}
		f_unlink(CURRENT_VOICE_FILE);
	}

	//Reservo memoria para los vectores de procesamiento
	template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	current_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(template == NULL || current_feature == NULL){
		vPortFree(template);
		vPortFree(current_feature);
		return false;
	}


	//Genero template
	process_features(feature_filepath, num_of_blocks, NUM_OF_VOICES, template, current_feature, FEATURE_SIZE);

	return true;
}
//static bool capture_and_save_voice(uint16_t *buffer, uint32_t size, char *filename){
//	display_message_t message;
//	bool result;
//	//Capturo la voz
//	message = DISPLAY_START_SPEECH_REC;
//	xQueueSend(display_queue, &message, portMAX_DELAY);
//	xSemaphoreTake(sd_display_sync, portMAX_DELAY);
//	capture_voice(buffer, size);
//
//	//Muestro mensaje de procesamiento
//	message = DISPLAY_PROCESSING_DATA;
//	xQueueSend(display_queue, &message, portMAX_DELAY);
//
//	//Guardo la voz y extraigo los features
//	result = store_voice(buffer, size, FRAME_SIZE, filename);
//	if(result){
//		result = extract_and_save_features(CURRENT_VOICE_FILE, filename);
//	}
//	return result;
//}
//
//bool generate_template(void){
//	display_message_t message;
//	float *template = NULL;
//    float *my_feature = NULL;
//    uint16_t *voice_buf = NULL;
//	char filenames[NUM_OF_VOICES][20] = {"feature_1.bin",
//										 "feature_2.bin",
//										 "feature_3.bin",
//										 "feature_4.bin",
//										 "feature_5.bin"};
//
//	//Reservo memoria para el buffer que contiene la captura de voz
//	voice_buf = pvPortMalloc(U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
//	if(voice_buf == NULL){
//		//Manejar el error
//		message = DISPLAY_ERROR_MEMORY;
//		xQueueSend(display_queue, &message, portMAX_DELAY);
//		return false;															//Error critico, no continuar
//	}
//
//	//Capturo las voces y las guardo en la tarjeta SD
//	for(uint8_t i = 0; i < NUM_OF_VOICES; i++){
//		//Envio mensaje de inicio de reconocimiento al display
//		message = DISPLAY_START_SPEECH_REC;
//		xQueueSend(display_queue, &message, portMAX_DELAY);
//		xSemaphoreTake(sd_display_sync, portMAX_DELAY);							//Bloqueo la tarea hasta que el display me devuelva el semaforo
//
//		//Capturo voces y las almaceno en la SD
//		capture_voice(voice_buf, AUDIO_BUFFER_SIZE);
//
//		//Muestro en el display que se estan procesando datos
//		message = DISPLAY_PROCESSING_DATA;
//		xQueueSend(display_queue, &message, portMAX_DELAY);
//
//		//Almaceno la voz y extraigo los features
//		if(!store_voice(voice_buf, AUDIO_BUFFER_SIZE, FRAME_SIZE, CURRENT_VOICE_FILE) || !extract_and_save_features(CURRENT_VOICE_FILE, filenames[i])){
//			message = DISPLAY_SD_WRITE_ERROR;
//			xQueueSend(display_queue, &message, portMAX_DELAY);
//			vPortFree(voice_buf);
//			return false;
//		}
//	}
//
//	//Libero memoria utilizada para capturar voz
//	vPortFree(voice_buf);
//
//	//Reservo memoria utilizada para crear el template
//	template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
//	my_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
//	if(template == NULL || my_feature == NULL){
//		//Manejar error
//		message = DISPLAY_ERROR_MEMORY;
//		xQueueSend(display_queue, &message, portMAX_DELAY);
//		vPortFree(template);
//		vPortFree(my_feature);
//		return false;
//	}
//
//	arm_fill_f32(0.0f, template, FEATURE_SIZE);
//
//	for(uint8_t i = 0; i < NUM_OF_FRAMES; i++){
//		//En cada iteracion se procesa el bloque i del feature_{j+1}.bin
//		for(uint8_t j = 0; j < NUM_OF_VOICES; j++){
//			if(!read_buffer_from_sd(filenames[j], my_feature, FEATURE_SIZE, i * FEATURE_SIZE)){
//				message = DISPLAY_SD_WRITE_ERROR;
//				xQueueSend(display_queue, &message, portMAX_DELAY);
//				vPortFree(template);
//				vPortFree(my_feature);
//				return false;
//			}
//			arm_add_f32(template, my_feature, template, FEATURE_SIZE);
//		}
//
//		//Escalo para obtener el promedio
//		arm_scale_f32(template, 1.0f / NUM_OF_VOICES, template, FEATURE_SIZE);
//		if(!save_buffer_on_sd(CURRENT_TEMPLATE_FILE, template, FEATURE_SIZE)){
//			message = DISPLAY_SD_WRITE_ERROR;
//			xQueueSend(display_queue, &message, portMAX_DELAY);
//			vPortFree(template);
//			vPortFree(my_feature);
//			return false;
//		}
//
//		arm_fill_f32(0.0f, template, FEATURE_SIZE);
//	}
//
//	//Borro los archivos generados para crear el template
//	for(uint8_t i = 0; i < NUM_OF_VOICES; i++){
//		f_unlink(filenames[i]);
//	}
//
//	//Libero memoria de template y my_feature
//	vPortFree(template);
//	vPortFree(my_feature);
//
//	return true;									//Template generado exitosamente
//}
