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


/**
 * @brief Captura 10 señales de voz que seran utilizadas para crear un template.
 *
 * @param None
 * @return true si se capturaron correctamente las voces, false en caso contrario.
 */

//Defines asociados al template
#define NUM_OF_TEMPLATE_SAMPLES 10
bool generate_template(void){
	uint16_t voice_buffer[AUDIO_BUFFER_SIZE] = {0};
	bool res;
	for(uint8_t i = 0; i < NUM_OF_TEMPLATE_SAMPLES; i++){
		//Envio mensaje al display para indicar que comienza reconocimiento de voz
		send_message(DISPLAY_START_SPEECH_REC, BLOCKING);

		//Capturo voz
		capture_voice(voice_buffer, AUDIO_BUFFER_SIZE);

		//Muestro en pantalla que se estan procesando los datos
		send_message(DISPLAY_PROCESSING_DATA, NON_BLOCKING);						//Se envia no bloqueante para seguir procesando datos mientras se muestra el mensaje

		//Envio voz por uart
		res = send_uart((uint8_t *) voice_buffer, U16_SIZE_BYTES(AUDIO_BUFFER_SIZE));
		if(!res) return false;
	}

	//Si se creo el template correctamente devuelve true
	return true;
}

