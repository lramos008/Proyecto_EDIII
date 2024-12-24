#include "sd_functions.h"
#include "capture_voice_functions.h"
#include "utils.h"


/*Este archivo contiene las funciones principales de la tarea SD*/
#define NO_DATABASE FR_NO_FILE
#define NO_REGISTER FR_DISK_ERR

FRESULT initialize_files(void){
	indicatorMessage message;
	FRESULT res;

	//Chequeo que exista el archivo con la database de usuarios
	res = check_if_file_exists("usuarios.txt");
	if(res != FR_OK){
		return NO_DATABASE;
	}

	//Chequeo si existe el archivo de registro de accesos. Si no existe, lo creo
	res = check_if_file_exists("registro.txt");
	if(res != FR_OK){
		res = create_file("registro.txt", "Fecha Usuario Estado\n");
		if(res != FR_OK){
			return NO_REGISTER;
		}
	}
	return FR_OK;
}

bool initialize_sd_and_verify_files(indicatorMessage *error_message){
	FRESULT res;

	res = mount_sd("");
	if(res != FR_OK){
		*error_message = PANTALLA_ERROR_MONTAJE_SD;
		return false;
	}

	res = initialize_files();
	if(res != FR_OK){
		switch(res){
		case NO_DATABASE:
			*error_message = PANTALLA_DATABASE_NO_EXISTE;
			break;
		case NO_REGISTER:
			*error_message = PANTALLA_REGISTRO_NO_CREADO;
			break;
		default:
			*error_message = PANTALLA_ERROR_DESCONOCIDO;
			break;
		}
		unmount_sd("");
		return false;
	}

	//No desmontar si se encontraron los archivos necesarios
	return true;
}

bool process_user_key(char *user_key, char *user_name, indicatorMessage *error_message){
	FRESULT res = search_user("usuarios.txt", user_key, user_name);
	if(res != FR_OK){
		*error_message = PANTALLA_USUARIO_NO_EXISTE;
		return false;
	}
	clear_char(user_name, '\r');
	return true;
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

bool recognize_user_voice(char *template_path, char *user_name, indicatorMessage *message){
	uint16_t *voice_buf = pvPortMalloc(VOICE_BUFFER_SIZE * sizeof(uint16_t));
	char *entry;
	if(voice_buf == NULL){
		*message = PANTALLA_ERROR_MEMORIA;
		return false;
	}

	capture_voice(voice_buf, VOICE_BUFFER_SIZE);
	store_voice(voice_buf, VOICE_BUFFER_SIZE, "current_voice.bin");
	vPortFree(voice_buf);

	//Extraigo los features de la voz
	entry = pvPortMalloc(ENTRY_STR_SIZE * sizeof(char));
	extract_and_save_features("current_voice.bin", "current_feature.bin");
	if(check_voice(template_path, "current_feature.bin")){
		build_entry_message(entry, user_name, "Concedido\n");
		write_entry("registro.txt", entry);
		*message = PANTALLA_VOZ_RECONOCIDA;
		vPortFree(entry);
		return true;
	}
	else{
		build_entry_message(entry, user_name, "Denegado");
		write_entry("registro.txt", entry);
		*message = PANTALLA_VOZ_NO_RECONOCIDA;
		return false;
	}
}
