
#include <fatfs_sd.h>
#include "ff.h"
#include <stdlib.h>
#include "common_utils.h"



#define UART &huart2
#define LINE_BUFFER_SIZE 128
#define MAX_RETRIES 20

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

extern RTC_HandleTypeDef hrtc;			//Handler del RTC


//Esta funcion monta la tarjeta SD para su uso
bool try_mount(FATFS *fs, char *path){
	FRESULT res;
	bool is_mounted = false;
	uint8_t attemps = 0;

	//Intento montar la tarjeta SD
	do{
		res = f_mount(fs, path, 1);
		if(res == FR_OK){
			is_mounted = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);				//Se intenta un numero de veces. Si falla todas devuelve false

	//Devuelvo estado de montaje
	return is_mounted;
}

//Esta funcion desmonta la tarjeta SD
bool try_unmount(char *path){
	FRESULT res;
	bool is_unmounted = false;
	uint8_t attemps = 0;

	//Intento desmontar la tarjeta SD
	do{
		res = f_mount(NULL, path, 1);
		if(res == FR_OK){
			is_unmounted = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);

	//Devuelvo estado de desmontaje
	return is_unmounted;
}

bool try_f_open(FIL *fil, char *path, BYTE mode){
	FRESULT res;
	uint8_t attemps = 0;
	bool is_open = false;

	//Intento abrir el archivo
	do{
		res = f_open(fil, path, mode);
		if(res == FR_OK){
			is_open = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);

	//Devuelvo estado de lectura
	return is_open;
}

bool try_f_close(FIL *fil){
	FRESULT res;
	uint8_t attemps = 0;
	bool is_closed = false;

	//Intento abrir el archivo
	do{
		res = f_close(fil);
		if(res == FR_OK){
			is_closed = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);

	//Devuelvo estado de lectura
	return is_closed;
}

//Esta funcion lee bytes de un archivo
bool try_f_read(FIL *fil, void *buffer, UINT btr, UINT *br){
	FRESULT res;
	uint8_t attemps = 0;
	bool is_read = false;

	//Intento leer desde SD
	do{
		res = f_read(fil, buffer, btr, br);
		if(res == FR_OK){
			is_read = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);

	//Devuelvo estado de lectura
	return is_read;
}

//Esta funcion escribe bytes en un archivo
bool try_f_write(FIL *fil, void *buffer, UINT btw, UINT *bw){
	FRESULT res;
	uint8_t attemps = 0;
	bool is_written = false;

	//Intento leer desde SD
	do{
		res = f_write(fil, buffer, btw, bw);
		if(res == FR_OK){
			is_written = true;
			break;
		}
		attemps++;
	}while(attemps < MAX_RETRIES);

	//Devuelvo estado de lectura
	return is_written;
}


bool check_if_file_exists(char *filename){
	FRESULT res;
	res = f_stat(filename, &fno);
	return (fresult != FR_OK) ? false : true;
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


bool save_data_on_sd(char *filename, void *buffer, uint32_t btw){
	bool res;
	//Abro el archivo
	res = try_f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
	if(!res){
		//No se pudo abrir el archivo
		return false;
	}

	//Escribo los datos del buffer en el archivo
	res = try_f_write(&fil, buffer, btw, &bw);
	if(!res || bw < btw){
		res = try_f_close(&fil);
		return false;
	}

	//Cierro el archivo
	res = try_f_close(&fil);
	return res ? true : false;
}

bool read_data_from_sd(char *filename, void *buffer, uint32_t btr, uint32_t current_pos){
	//Current pos debe ser multiplicado por el numero de bytes de un float, o guardar la ultima posicion
	bool res;
	//Abro el archivo
	res = try_f_open(&fil, filename, FA_READ);
	if(!res){
		//No se pudo abrir el archivo
		return false;
	}

	//Posiciono el puntero de lectura
	fresult = f_lseek(&fil, current_pos);
	if(fresult != FR_OK){
		try_f_close(&fil);
		return false;
	}

	//Leo datos desde archivo y los guardo en un buffer
	res = try_f_read(&fil, buffer, btr, &br);
	if(!res){
		res = try_f_close(&fil);
		return false;
	}

	//Cierro el archivo
	res = try_f_close(&fil);
	return res ? true : false;

}




FRESULT write_entry(char *filename, char *entry){
	FIL fil;
	/*Abro el archivo en modo append*/
	fresult = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
	if(fresult == FR_OK){
		char *buffer = pvPortMalloc(70 * sizeof(char));
		snprintf(buffer, 70, "%s\n", entry);
		/*Escribo la entrada en el archivo*/
		fresult = f_write(&fil, buffer, strlen(buffer), &bw);
		if(fresult != FR_OK || bw < strlen(buffer)){
			return FR_DISK_ERR;
		}
		vPortFree(buffer);
		fresult = f_close(&fil);
	}
	return fresult;
}

FRESULT search_user(char *filename, char *user_key, char *user_name){
	char line[LINE_BUFFER_SIZE];
	char *line_key;
	char *line_user;
	//Abro el archivo
	fresult = f_open(&fil, filename, FA_READ);
	if(fresult != FR_OK){
		return fresult;
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
				f_close(&fil);
				return FR_OK;
			}
		}

		//Limpio la linea para que no se acumulen caracteres de palabras largas
		clear_buffer(line, LINE_BUFFER_SIZE);
	}

	//Si se llego al final sin encontrar el usuario, devuelve mensaje de error
	fresult = f_close(&fil);
	return FR_NO_FILE;												//Devuelve FR_NO_FILE si no se encuentra el usuario
}








