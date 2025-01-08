#include "common_utils.h"
#include "sd_functions.h"

#define DATABASE "usuarios.txt"
#define REGISTER "registro.txt"

bool initialize_sd_and_verify_files(display_message_t *error_message){
	FRESULT res;
	res = mount_sd("");
	if(res != FR_OK){
		*error_message = DISPLAY_INIT_ERROR;
		return false;
	}

	//Verifico existencia de la base de datos
	res = check_if_file_exists(DATABASE);
	if(res != FR_OK){
		*error_message = DISPLAY_DATABASE_NOT_FOUND;
		unmount_sd("");
		return false;
	}

	//Verifico existencia del registro de acceso, o lo creo en su defecto
	res = check_if_file_exists(REGISTER);
	if(res != FR_OK){
		res = create_file(REGISTER, "Fecha Usuario Estado\n");
		if(res != FR_OK){
			*error_message = DISPLAY_REGISTER_NOT_CREATED;
			unmount_sd("");
			return false;
		}
	}

	//Archivos y SD inicializados correctamente
	return true;
}


bool process_user_key(char *user_key, char *user_name){
	FRESULT res = search_user(DATABASE, user_key, user_name);
	if(res != FR_OK){
		return false;
	}
	clear_char(user_name, '\r');										//Limpio '\r' agregado por windows
	return true;														//Usuario encontrado
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
