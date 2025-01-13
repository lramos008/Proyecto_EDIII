#ifndef SAVE_AND_READ_DATA_H
#define SAVE_AND_READ_DATA_H
#include "common_utils.h"

/*===========================[Public function prototypes]=============================*/
bool save_data_on_sd(char *filename, void *buffer, uint32_t btw);
bool read_data_from_sd(char *filename, void *buffer, uint32_t btr, uint32_t *last_pos);
#endif // SAVE_AND_READ_DATA_H
