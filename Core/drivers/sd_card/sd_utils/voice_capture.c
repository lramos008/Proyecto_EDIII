#include "processing.h"
#include "sd_functions.h"

#define ADC_RESOLUTION 4096.0
#define VOLTAGE_REFERENCE 3.3
/*================[Extern variables]=====================*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;
extern volatile bool conv_cplt_flag;

/*================[Public functions]=====================*/
void capture_voice(uint16_t *buffer, uint32_t size){
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, size);					//Inicio captura de datos con ADC
	while(!conv_cplt_flag);												//Espero a que termine la captura
	conv_cplt_flag = false;
	return;
}

static void get_voltage(uint16_t *in_buffer, float *out_buffer, uint32_t size){
	for(uint32_t i = 0; i < size; i++){
		out_buffer[i] = VOLTAGE_REFERENCE * ((float)in_buffer[i]) / ADC_RESOLUTION;
	}
}

bool store_voice(uint16_t *voice_buffer, uint32_t buf_size, uint32_t frame_size, char *filename){
	display_message_t message;
	float *current_frame;
	//Verifico que size sea multiplo de frame_size
	if((buf_size % frame_size) != 0){
		//Manejar error
		return false;
	}

	//Reservo memoria para el bloque de procesamiento
	uint32_t num_of_frames = buf_size / frame_size;
	current_frame = pvPortMalloc(FLOAT_SIZE_BYTES(frame_size));
	if(current_frame == NULL){
		//Manejar este caso, enviar un print
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		return false;
	}

	for(uint8_t i = 0; i < num_of_frames; i++){
		//Convierto los valores obtenidos a tension
		get_voltage(&voice_buffer[i * frame_size], current_frame, frame_size);

		//Guardo en la SD
		save_buffer_on_sd(filename, current_frame, frame_size);
	}

	//Libero memoria utilizada
	vPortFree(current_frame);
	return true;											//Ejecucion exitosa de la funcion
}

bool extract_and_save_features(char *voice_name, char *feature_name){
	display_message_t message;
	float *current_frame = pvPortMalloc(FLOAT_SIZE_BYTES(FRAME_SIZE));
	float *feature_frame = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(current_frame == NULL || feature_frame == NULL){
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		vPortFree(current_frame);
		vPortFree(feature_frame);
		return false;
	}
	for(uint8_t i = 0; i < NUM_OF_FRAMES; i++){
		read_buffer_from_sd(voice_name, current_frame, FRAME_SIZE, i * FRAME_SIZE);
		if(!process_frame(current_frame, feature_frame, FRAME_SIZE)){
			message = DISPLAY_ERROR_MEMORY;
			xQueueSend(display_queue, &message, portMAX_DELAY);
			vPortFree(current_frame);
			vPortFree(feature_frame);
			return false;
		}
		save_buffer_on_sd(feature_name, feature_frame, FEATURE_SIZE);
	}

	//Elimino el archivo de voz para quedarme solo con los features
	f_unlink(voice_name);

	//Libero memoria utilizada
	vPortFree(current_frame);
	vPortFree(feature_frame);
	return true;											//Ejecucion exitosa de la funcion
}

bool check_voice(char *template_path, char *feature_path){
	display_message_t message;
	float32_t *template = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	float32_t *extracted_feature = pvPortMalloc(FLOAT_SIZE_BYTES(FEATURE_SIZE));
	if(template == NULL || extracted_feature == NULL){
		message = DISPLAY_ERROR_MEMORY;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}
	bool compare_res = false;
	bool is_recognized = false;
	uint8_t frame_counter = 0;
	for(uint8_t i = 0; i < NUM_OF_FRAMES; i++){
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
