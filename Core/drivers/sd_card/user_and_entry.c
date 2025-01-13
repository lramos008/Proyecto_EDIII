#include "common_utils.h"
#include "sd_functions.h"

#define LINE_BUFFER_SIZE 128

extern RTC_HandleTypeDef hrtc;			//Handler del RTC
extern FATFS fs;  // file system
extern FIL fil; // File
extern FILINFO fno;
extern FRESULT fresult;  // result
extern UINT br, bw;  // File read/write count


bool check_if_file_exists(char *filename){
	FRESULT res;
	res = f_stat(filename, &fno);
	return (res != FR_OK) ? false : true;
}


bool create_file(char *filename, char *header){
	bool res;
	/*Creo el archivo*/
	res = try_f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(!res){
		//No se pudo abrir el archivo
		return false;
	}

	/*Escribo el header*/
	res = try_f_write(&fil, header, strlen(header), &bw);
	if(!res || bw < strlen(header)){
		res = try_f_close(&fil);
		return false;
	}
	/*Cierro el archivo*/
	res = try_f_close(&fil);
	return res ? true : false;
}

void get_time_from_rtc(char *rtc_lecture){
	/***Esta funcion devuelve en formato string la fecha y hora actual usando el RTC***/
	RTC_TimeTypeDef currentTime;
	RTC_DateTypeDef currentDate;
	char time[15];
	/*Obtengo el tiempo actual*/
	HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
	/*Obtengo la fecha actual*/
	HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);
	/*Guardo fecha y hora en los buffers correspondientes*/
	snprintf(time, 15, "%02d:%02d:%02d ", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
	snprintf(rtc_lecture, 15, "%02d-%02d-%2d ", currentDate.Date, currentDate.Month, 2000 + currentDate.Year);
	strcat(rtc_lecture, time);
	return;
}

bool write_entry (char *filename, char *entry){
	FIL fil;
	bool res;
	bool is_written = true;
	//Abro archivo en modo append
	res = try_f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
	if(!res){
		is_written = false;
		return is_written;
	}

	//Escribo la entrada en el archivo de registro
	res = try_f_write(&fil, entry, strlen(entry), &bw);
	if(!res || bw < strlen(entry)){
		if(res){
			//Si se escribieron bytes de menos, elimino el archivo
			f_unlink(filename);
		}
		is_written = false;
	}

	//Cierro el archivo
	res = try_f_close(&fil);
	if(!res){
		is_written = false;
	}
	return is_written;
}


bool search_user(char *filename, char *user_key, char *user_name){
	char line[LINE_BUFFER_SIZE];
	char *line_key;
	char *line_user;
	bool is_found;
	bool res;

	//Abro el archivo en modo lectura
	res = try_f_open(&fil, filename, FA_READ);
	if(!res){
		is_found = false;;
		return is_found;
	}

	//Descarto el header
	f_gets(line, LINE_BUFFER_SIZE, &fil);

	//Busco el usuario asociado linea por linea
	while(f_gets(line, LINE_BUFFER_SIZE, &fil) != NULL){
		line_key = strtok(line, " ");								//Separo la clave
		line_user = strtok(NULL, "\n");								//Separo el usuario

		//Comparo clave
		if(line_key && strcmp(line_key, user_key) == 0){
			if(line_user){
				strncpy(user_name, line_user, USER_STR_SIZE - 1);
				user_name[USER_STR_SIZE - 1] = '\0';				//Me aseguro de que haya terminacion nula
				res = try_f_close(&fil);
				is_found =  res ? true : false;
				return is_found;
			}
		}
		//Limpio la linea para que no se acumulen caracteres de palabras largas
		clear_buffer(line, LINE_BUFFER_SIZE);
	}

	//Si se llego al final del archivo y no se encontro el usuario, devuelve false
	res = try_f_close(&fil);
	is_found = false;
	return is_found;
}

