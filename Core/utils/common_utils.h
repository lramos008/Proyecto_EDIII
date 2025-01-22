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
#define USER_STR_SIZE 40
#define TEMPLATE_STR_SIZE 50
#define ENTRY_STR_SIZE 60

//Defines asociados al procesamiento de la señal
#define AUDIO_BUFFER_SIZE 	24576
#define BLOCK_SIZE 		   	2048
#define FEATURE_SIZE   		(BLOCK_SIZE / 2)
#define OVERLAP				(BLOCK_SIZE / 2)						//Overlap del 50%
#define NUM_OF_BLOCKS 		(AUDIO_BUFFER_SIZE - OVERLAP) / OVERLAP

//Defines asociados al template
#define NUM_OF_TEMPLATE_SAMPLES 10
//Defines asociados al reconocimiento de voz
#define DISTANCE_THRESHOLD 2.3f
#define ACCEPTED_BLOCK_TH  (NUM_OF_BLOCKS - 2)

//Defines relacionados al envio de mensajes al display
#define BLOCKING true
#define NON_BLOCKING false
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
	DISPLAY_PROCESSING_DATA,							//Procesando datos
	DISPLAY_ACCESS_GRANTED,								//Acceso concedido
	DISPLAY_ACCESS_DENIED,								//Acceso denegado
	DISPLAY_TIMEOUT_EVENT,								//Timeout. Se tardo mucho tiempo en ingresar digitos
	DISPLAY_INCOMPLETE_SEQUENCE_EVENT,					//Se ingreso secuencia incompleta
	DISPLAY_TEMPLATE_SAVED,								//Se guardo el template correctamente
	//Pantallas asociadas a los errores
	DISPLAY_TEMPLATE_NOT_CREATED,						//No se pudo crear el template
	DISPLAY_MOUNTING_ERROR,								//No se pudo montar la tarjeta SD
	DISPLAY_TEMPLATE_NOT_FOUND,							//No se encuentra template
	DISPLAY_DATABASE_NOT_FOUND,							//No se encuentra base de datos
	DISPLAY_REGISTER_NOT_CREATED,						//No se pudo crear el registro de accesos
	DISPLAY_MEMORY_ERROR,								//Error al intentar reservar memoria con pvPortMalloc
	DISPLAY_READ_SD_ERROR,								//Error al querer leer buffer desde tarjeta SD
	DISPLAY_WRITE_SD_ERROR								//Error al querer escribir buffer en la tarjeta SD
}display_message_t;
/*========================[Public function prototypes]===========================*/
void clear_buffer(char *buffer, size_t length);
void clear_char(char *buffer, char character);
void send_message(display_message_t message, bool is_blocking);
void send_error(display_message_t error_message);
#endif // COMMON_UTILS_H
