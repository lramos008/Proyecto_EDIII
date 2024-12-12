#ifndef UTILS_H
#define UTILS_H
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
/*======================[Extern variables]==========================*/
extern QueueHandle_t display_queue;
extern QueueHandle_t sequence_queue;
/*=======================[Public defines]==========================*/
#define MAX_DIGITS 6
#define SEQUENCE_LENGTH 6
#define USER_STR_SIZE 50
/*========================[Public macros]==========================*/
#define FLOAT_SIZE_BYTES(X) ((X) * sizeof(float))
/*====================== [Public Typedefs]=========================*/
typedef uint8_t indicatorMessage;
/*========================[Public enums]===========================*/
enum{												//Para nombrar las pantallas del display
	PANTALLA_IDLE,
	PANTALLA_DE_INICIO,
	PANTALLA_INGRESO_KEYPAD,
	PANTALLA_BORRAR_KEYPAD,
	PANTALLA_USUARIO_ENCONTRADO,
	PANTALLA_USUARIO_NO_EXISTE,
	PANTALLA_RECONOCIMIENTO_DE_VOZ,
	PANTALLA_VOZ_RECONOCIDA,
	PANTALLA_VOZ_NO_RECONOCIDA,
	PANTALLA_ACCESO_CONCEDIDO,
	PANTALLA_TIMEOUT,
	PANTALLA_SECUENCIA_INCOMPLETA
};

typedef enum{										//Para nombrar los estados de la adquisicion con el keypad
	ESPERANDO_DIGITO_1,
	ESPERANDO_DIGITO_2,
	ESPERANDO_DIGITO_3,
	ESPERANDO_DIGITO_4,
	ESPERANDO_DIGITO_5,
	ESPERANDO_DIGITO_6,
	ESPERANDO_CONFIRMACION,
	BUSQUEDA_DE_USUARIO,
	SECUENCIA_INCOMPLETA,
	TIMEOUT
}keypad_state_t;

enum {INGRESAR = 0, BORRAR};						//Para ingresar / borrar digitos en el display
/*========================[Public function prototypes]===========================*/
void clear_buffer(char *buffer, size_t length);
#endif /* UTILS_H */
