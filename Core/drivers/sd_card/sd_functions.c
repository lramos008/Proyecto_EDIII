#include "main.h"
#include "fatfs.h"
#include "ff.h"
#include "fatfs_sd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define UART &huart2

#define LINE_SIZE 100

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

void mount_sd(const char* path){
	fresult = f_mount(&fs, path, 1);

	return;
}

void unmount_sd(const char* path){
	fresult = f_mount(NULL, path, 1);
	if(fresult != FR_OK){
		send_uart("Error al desmontar la tarjeta SD!!!\n\n");
		while(1);
	}
}



FRESULT check_if_file_exists(char *filename){
	fresult = f_stat(filename, &fno);
	return (fresult != FR_OK) ? FR_NO_FILE : fresult;
}


FRESULT create_file(char *filename, char *header){
	/*Creo el archivo*/
	fresult = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(fresult != FR_OK) return fresult;
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
	snprintf(time, 15, "%02d:%02d:%02d", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
	snprintf(rtc_lecture, 15, "%02d-%02d-%2d ", currentDate.Date, currentDate.Month, 2000 + currentDate.Year);
	strcat(rtc_lecture, time);
	vPortFree(time);
	return;
}

FRESULT save_buffer_on_sd(char *filename, uint16_t *buffer, uint16_t buf_size){
	char line[20];
	fresult = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(fresult != FR_OK){
		return fresult;
	}
	/*Escribo cada dato del buffer en una linea separada*/
	for(uint16_t i = 0; i < buf_size; i++){
		/*Formateo numero como string*/
		snprintf(line, 20, "%u\n", buffer[i]);
		/*Escribo en el archivo*/
		fresult = f_write(&fil, line, strlen(line), &bw);
		if (fresult != FR_OK || bw < strlen(line)) {
			f_close(&fil); // Cerrar el archivo antes de retornar
		    return fresult;  // Error al escribir en el archivo
		}
	}
	f_close(&fil);
	return FR_OK;
}

void read_voice_buffer(char *filename, uint16_t *buffer, uint16_t length){
	char line[5];
	FRESULT fresult;
	static uint32_t current_pos = 0;
	uint32_t count = 0;
	/*Abro el archivo*/
	fresult = f_open(&fil, filename, FA_READ);
	if(fresult != FR_OK){
		send_uart("Error al abrir el archivo current_voice.txt.\n");
		unmount_sd("/");
		while(1);
	}
	/*Posiciono el cursor en la posicion inicial a leer*/
	fresult = f_lseek(&fil, current_pos);
	if(fresult != FR_OK){
		send_uart("Error al mover el puntero del archivo.\n");
		f_close(&fil);
		unmount_sd("/");
		while(1);
	}
	while(count < length){
		/*Leo una linea del archivo y verifico que no este vacia*/
		if(f_gets(line, sizeof(line), &fil) != NULL){
			if(strlen(line) > 1){
				/*Si cumple con el largo esperado, guardar el valor en el buffer*/
				buffer[count] = (uint16_t) strtoul(line, NULL, 10);
				count++;
			}
		}
		else{
			/*Si alcanzo el final del archivo salgo del bucle*/
			break;
		}
	}
	current_pos = f_tell(&fil);
	f_close(&fil);
	return;
}

void read_template(char *filename, float *buffer, uint16_t length){
	char line[9];
	FRESULT fresult;
	static uint32_t current_pos = 0;
	uint32_t count = 0;
	float value;
	/*Abro el archivo*/
	fresult = f_open(&fil, filename, FA_READ);
	if(fresult != FR_OK){
		send_uart("Error al abrir el archivo template.\n");
		unmount_sd("/");
		while(1);
	}
	/*Posiciono el cursor en la posicion inicial a leer*/
	fresult = f_lseek(&fil, current_pos);
	if(fresult != FR_OK){
		send_uart("Error al mover el puntero del archivo.\n");
		f_close(&fil);
		unmount_sd("/");
		while(1);
	}
	while(count < length){
		/*Leo una linea del archivo y verifico que no este vacia*/
		if(f_gets(line, 9, &fil) != NULL){
			if(strlen(line) > 1){
				/*Si cumple con el largo esperado, guardar el valor en el buffer*/
				value = strtof(line, NULL);
				buffer[count] = value;
				count++;
			}
		}
		else{
			/*Si alcanzo el final del archivo salgo del bucle*/
			break;
		}
	}
	current_pos = f_tell(&fil);
	f_close(&fil);
	return;
}

FRESULT write_entry(char *filename, char *entry){
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
		f_close(&fil);
	}
	return fresult;
}

FRESULT look_for_user(char *filename, char *username, uint32_t sequence){
	char line[LINE_SIZE];					//Buffer para leer las lineas del archivo
	char *token;
	uint32_t line_sequence;
	/*Abro el archivo*/
	fresult = f_open(&fil, filename, FA_READ);
	if(fresult != FR_OK) return fresult;
	/*Descarto linea de header del archivo*/
	f_gets(line, LINE_SIZE, &fil);
	/*Busco linea por linea en el archivo*/
	while(f_gets(line, LINE_SIZE, &fil) != NULL){
		/*Separo clave de nombre de usuario*/
		token = strtok(line, " ");
		if(token != NULL){
			/*Convierto la clave string a entero sin signo*/
			line_sequence = (uint32_t) atoi(token);
			/*Obtengo usuario*/
			token = strtok(NULL, " ");
			if(token != NULL){
				if(line_sequence == sequence){
					/*Si hay usuario, y concuerdan las claves, guardo el usuario*/
					strcpy(username, token);
					fresult = f_close(&fil);
					return FR_OK;
				}
			}

		}
	}
	/*Devuelve FR_NO_FILE si no hay usuario*/
	return FR_NO_PATH;
}




