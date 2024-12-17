
#include <fatfs_sd.h>
#include "ff.h"
#include <stdlib.h>
#include "utils.h"



#define UART &huart2

#define LINE_BUFFER_SIZE 128

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;			//Handler del RTC


void send_uart(char *string){
	HAL_UART_Transmit(UART, (uint8_t *)string, strlen (string), HAL_MAX_DELAY);
	return;
}

void send_buffer_via_uart(uint16_t *buffer, size_t size){
	char str[10];
	for(size_t i = 0; i < size; i++){
		snprintf(str, 10, "%u\n", buffer[i]);
		send_uart(str);
	}
	return;
}

void mount_sd(char* path){
	fresult = f_mount(&fs, path, 1);
	if(fresult != FR_OK){
		send_uart("Error al montar la tarjeta SD!!!\n");
		while(1);
	}
	return;
}

void unmount_sd(char* path){
	fresult = f_mount(NULL, path, 1);
	if(fresult != FR_OK){
		send_uart("Error al desmontar la tarjeta SD!!!\n");
		while(1);
	}
}


FRESULT check_if_file_exists(char *filename){
	fresult = f_stat(filename, &fno);
	return (fresult != FR_OK) ? FR_NO_FILE : fresult;
}


FRESULT create_file(char *filename, char *header){
	FIL fil;
	/*Creo el archivo*/
	fresult = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(fresult != FR_OK){
		return fresult;
	}
	/*Escribo el header*/
	fresult = f_write(&fil, header, strlen(header), &bw);
	if(fresult != FR_OK || bw < strlen(header)){
		f_close(&fil);
		return (fresult != FR_OK) ? fresult : FR_DISK_ERR;
	}
	/*Cierro el archivo*/
	fresult = f_close(&fil);
	return fresult;
}

void get_time_from_rtc(char *rtc_lecture){
	/***Esta funcion devuelve en formato string la fecha y hora actual usando el RTC***/
	RTC_TimeTypeDef currentTime;
	RTC_DateTypeDef currentDate;
	char *time = pvPortMalloc(15 * sizeof(char));
	/*Obtengo el tiempo actual*/
	HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
	/*Obtengo la fecha actual*/
	HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);
	/*Guardo fecha y hora en los buffers correspondientes*/
	snprintf(time, 15, "%02d:%02d:%02d ", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
	snprintf(rtc_lecture, 15, "%02d-%02d-%2d ", currentDate.Date, currentDate.Month, 2000 + currentDate.Year);
	strcat(rtc_lecture, time);
	vPortFree(time);
	return;
}

FRESULT save_buffer_on_sd(char *filename, float *buffer, size_t size){
	/* Guarda los valores de un vector float con tamaño size en el archivo filename.
	 * Es importante que el archivo sea .bin, ya que es más conveniente guardar
	 * los datos en este formato (ocupa menos espacio y es más directa la escritura).
	 */
	//Abro el archivo
	FIL fil;
	fresult = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);						//Se abre con append por si es necesario seguir escribiendo desde el final
	if(fresult != FR_OK){
		return fresult;
	}
	//Escribo los datos del buffer en el archivo
	fresult = f_write(&fil, buffer, FLOAT_SIZE_BYTES(size), &bw);
	if(fresult != FR_OK || bw < FLOAT_SIZE_BYTES(size)){
		f_close(&fil);																	//Frente a un error cierro el archivo y salgo de la funcion
		return (fresult != FR_OK) ? fresult : FR_DISK_ERR;								//Si se escribieron bytes de menos, devuelve FR_DISK_ERR
	}

	//Cierro el archivo
	fresult = f_close(&fil);
	return FR_OK;
}

FRESULT read_buffer_from_sd(char *filename, float *buffer, size_t size, uint32_t current_pos){
	/* Lee un bloque de valores float con tamaño size desde el archivo filename
	 * y los guarda en un vector. Es importante que el archivo sea .bin.
	 * La funcion esta pensada para leer de a bloques iguales, no guarda la ultima
	 * posicion del puntero de lectura, se debe llevar cuenta externamente.
	 */

	//Abro el archivo
	fresult = f_open(&fil, filename, FA_READ);
	if(fresult != FR_OK){
		return fresult;
	}

	//Posiciono el puntero de lectura en el archivo
	fresult = f_lseek(&fil, current_pos);
	if(fresult != FR_OK){
		f_close(&fil);
		return fresult;
	}

	//Leo el bloque de tamaño size desde el archivo
	fresult = f_read(&fil, buffer, FLOAT_SIZE_BYTES(size), &br);
	if(fresult != FR_OK){
		f_close(&fil);
		return fresult;
	}

	//Cierro el archivo
	fresult = f_close(&fil);
	return FR_OK;
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

FRESULT check_for_dir(char *filename){
	DIR dir;
	FRESULT res;
	fresult = f_opendir(&dir, filename);
	if(fresult != FR_OK){
		//Si no existe el directorio, lo creo
		fresult = f_mkdir(filename);
		if(fresult != FR_OK){
			//Devuelve FR_NO_PATH si no pudo crearse
			res = FR_NO_PATH;
		}
		else{
			//Si se pudo crear el directorio, devuelvo OK
			res = FR_OK;
		}
	}
	else{
		//Si existe, cierro el directorio y devuelve OK
		f_closedir(&dir);
		res = FR_OK;
	}
	return res;
}




