#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*======================[Extern variables]==========================*/
extern QueueHandle_t display_queue;
extern QueueHandle_t sequence_queue;
extern SemaphoreHandle_t keypad_sd_sync;
extern SemaphoreHandle_t sd_display_sync;

/*========================[Public defines]=========================*/
#define SEQUENCE_LENGTH 6
#define USER_STR_SIZE 50
#define TEMPLATE_STR_SIZE 50
#define ENTRY_STR_SIZE 60

#define AUDIO_BUFFER_SIZE 	18432
#define FRAME_SIZE 		   	2048
#define FEATURE_SIZE   		(FRAME_SIZE / 2)
#define NUM_OF_FRAMES 	   	(AUDIO_BUFFER_SIZE / FRAME_SIZE)
#define NUM_OF_VOICES 5
/*========================[Public macros]==========================*/
#define FLOAT_SIZE_BYTES(X) ((X) * sizeof(float))
#define CHAR_SIZE_BYTES(X) ((X) * sizeof(char))
#define U16_SIZE_BYTES(X) ((X) * sizeof(uint16_t))
/*====================== [Public Typedefs]=========================*/
typedef enum{
	DISPLAY_IDLE,										//Pantalla idle (no se envia nada)
	DISPLAY_SCREEN_WELCOME,								//Pantalla de inicio
	DISPLAY_ENTER_DIGIT,								//Ingreso de digitos (secuencia)
	DISPLAY_ERASE_DIGIT,								//Borrado de digitos (secuencia)
	DISPLAY_USER_FOUND,									//Usuario encontrado
	DISPLAY_USER_NOT_FOUND,								//Usuario no encontrado
	DISPLAY_START_SPEECH_REC,							//Comienzo de speech recognition (cuenta regresiva)
	DISPLAY_VOICE_RECOGNIZED,							//Voz reconocida
	DISPLAY_VOICE_NOT_RECOGNIZED,						//Voz no reconocida
	DISPLAY_ACCESS_GRANTED,								//Acceso concedido
	DISPLAY_TIMEOUT_EVENT,								//Timeout. Se tardo mucho tiempo en ingresar digitos
	DISPLAY_INCOMPLETE_SEQUENCE_EVENT,					//Se ingreso secuencia incompleta
	DISPLAY_PROCESSING_DATA,							//Procesando datos
	DISPLAY_TEMPLATE_SAVED,								//Se guardo el template correctamente
	//Pantallas asociadas a los errores
	DISPLAY_TEMPLATE_NOT_FOUND,							//No se encuentra template
	DISPLAY_DATABASE_NOT_FOUND,							//No se encuentra base de datos
	DISPLAY_REGISTER_NOT_CREATED,						//No se pudo crear el registro de accesos
	DISPLAY_ERROR_SD,									//Problema al montar la tarjeta SD
	DISPLAY_ERROR_MEMORY,								//Problema al asignar memoria dinamicamente
	DISPLAY_ERROR_UNKNOWN								//Error desconocido
}display_message_t;
/*========================[Public function prototypes]===========================*/
void clear_buffer(char *buffer, size_t length);
void clear_char(char *buffer, char character);

#endif // COMMON_UTILS_H
