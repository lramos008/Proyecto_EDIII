
#include <fatfs_sd.h>
#include "ff.h"
#include <stdlib.h>
#include "common_utils.h"

#define MAX_RETRIES 255

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

//Esta funcion monta la tarjeta SD para su uso
bool try_mount(char *path){
	FRESULT res;
	bool is_mounted = false;
	uint8_t attemps = 0;

	//Intento montar la tarjeta SD
	do{
		res = f_mount(&fs, path, 1);
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









