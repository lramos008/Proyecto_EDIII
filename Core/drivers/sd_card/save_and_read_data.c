#include "common_utils.h"
#include "sd_functions.h"

extern FATFS fs;  // file system
extern FIL fil; // File
extern FILINFO fno;
extern FRESULT fresult;  // result
extern UINT br, bw;  // File read/write count


//Funcion para escribir el contenido de buffers en un archivo
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

//Funcion para leer datos desde archivo y guardarlos en un buffer
bool read_data_from_sd(char *filename, void *buffer, uint32_t btr, uint32_t *last_pos){
	//Current pos debe ser multiplicado por el numero de bytes de un float, o guardar la ultima posicion
	bool res;
	//Abro el archivo
	res = try_f_open(&fil, filename, FA_READ);
	if(!res){
		//No se pudo abrir el archivo
		return false;
	}

	//Posiciono el puntero de lectura
	fresult = f_lseek(&fil, *last_pos);
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

	//Guardo la ultima posicion
	*last_pos = f_tell(&fil);

	//Cierro el archivo
	res = try_f_close(&fil);
	return res ? true : false;

}
