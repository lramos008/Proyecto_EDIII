#include "common_utils.h"
#include "sd_functions.h"
#include "user_and_entry.h"

#define DATABASE "usuarios.txt"
#define REGISTER "registro.txt"

bool initialize_sd_and_verify_files(display_message_t *error_message){
	bool res;
	//Verifico existencia de la base de datos
	res = check_if_file_exists(DATABASE);
	if(!res){
		*error_message = DISPLAY_DATABASE_NOT_FOUND;
		res = try_unmount("");
		return false;
	}

	//Verifico existencia del registro de acceso, o lo creo en su defecto
	res = check_if_file_exists(REGISTER);
	if(!res){
		res = create_file(REGISTER, "Fecha Usuario Estado\n");
		if(!res){
			*error_message = DISPLAY_REGISTER_NOT_CREATED;
			res = try_unmount("");
			return false;
		}
	}

	//Archivos y SD inicializados correctamente
	return true;
}


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
