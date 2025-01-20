#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H
#include "ff.h"
#include "common_utils.h"


/*===============[Public functions]=====================*/
bool try_mount(char *path);
bool try_unmount(char *path);
bool try_f_open(FIL *fil, char *path, BYTE mode);
bool try_f_close(FIL *fil);
bool try_f_read(FIL *fil, void *buffer, UINT btr, UINT *br);
bool try_f_write(FIL *fil, void *buffer, UINT btw, UINT *bw);
uint32_t get_file_size(char *filename);
#endif // SD_FUNCTIONS_H
