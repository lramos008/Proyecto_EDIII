#include "common_utils.h"
#include "sd_functions.h"
#include "user_and_entry.h"

#define DATABASE "usuarios.txt"
#define REGISTER "registro.txt"

/**
 * @brief Verifica la existencia los archivos importantes del control de acceso.
 *
 * Esta funcion verifica la existencia de la base de datos de usuario y del
 * registro de accesos en la tarjeta SD. En el caso del registro, si no existiese
 * se lo crea.
 *
 * @param error_message Puntero a variable display_message_t usada para enviar mensajes al display.
 * @return true si los archivos estan en la memoria SD, false si no los encuentra o no los puede crear.
 */
bool initialize_sd_and_verify_files(display_message_t *error_message){
	bool res;
	//Verifico existencia de la base de datos
	res = check_if_file_exists(DATABASE);
	if(!res){
		*error_message = DISPLAY_DATABASE_NOT_FOUND;
		return false;
	}

	//Verifico existencia del registro de acceso, o lo creo en su defecto
	res = check_if_file_exists(REGISTER);
	if(!res){
		res = create_file(REGISTER, "Fecha Usuario Estado\n");
		if(!res){
			*error_message = DISPLAY_REGISTER_NOT_CREATED;
			return false;
		}
	}

	//Archivos y SD inicializados correctamente
	return true;
}

/**
 * @brief Busca usuario en la base de datos que este asociado a la clave ingresada.
 *
 * Esta funcion recibe la clave ingresada por el keypad y busca en la base de datos
 * el usuario asociado a esta. Si existe, devuelve el usuario con el puntero *user_name,
 * y devuelve true con el return de la funcion. Caso contrario devuelve false.
 *
 * @param user_key Puntero char que apunta al inicio de la secuencia.
 * @param user_name Puntero al inicio del vector que contendra el nombre de usuario.
 * @return true si se encontro el usuario, false en caso contrario.
 */
bool process_user_key(char *user_key, char *user_name){
	//Busco el usuario
	if(!search_user(DATABASE, user_key, user_name)){
		//Devuelve false si no lo encuentra
		return false;
	}

	//Usuario encontrado
	clear_char(user_name, '\r');										//Limpio '\r' agregado por windows
	return true;														//True indica usuario encontrado
}


/**
 * @brief Arma la entrada del registro de acuerdo al estado de acceso.
 *
 * Esta funcion arma la entrada del registro. Incluye hora de acceso, nombre de usuario si se lo encontro
 * (o desconocido si no), y el status de acceso.
 *
 * @param entry Puntero char al vector que contendra la entrada del registro.
 * @param user_name Puntero char al vector que contiene el nombre de usuario (si es que se tiene).
 * @param status String con el estado del acceso (Concedido o Denegado).
 * @return None
 */
void build_entry_message(char *entry, char *user_name, const char *status){
	get_time_from_rtc(entry);						//Obtengo fecha y hora
	if(user_name != NULL){
		strcat(entry, user_name);					//Concateno nombre de usuario
	}
	else{
		strcat(entry, "Desconocido");				//Si no existe, concateno desconocido
	}
	strcat(entry, " ");
	strcat(entry, status);							//Concateno estado de acceso
	strcat(entry, "\n");
	return;
}
