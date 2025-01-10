#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H
#include "ff.h"
#include "common_utils.h"


/*===============[Public functions]=====================*/
void send_uart(char *string);
bool try_mount(char *path);
bool try_unmount(char *path);
FRESULT check_if_file_exists(char *filename);
FRESULT create_file(char *filename, char *header);
FRESULT save_u16_buffer(char *filename, uint16_t *buffer, uint32_t size);
FRESULT read_u16_file(char *filename, uint16_t *buffer, uint32_t size, uint32_t current_pos);
FRESULT save_buffer_on_sd(char *filename, float *buffer, size_t size);
FRESULT read_buffer_from_sd(char *filename, float *buffer, size_t size, uint32_t current_pos);
FRESULT search_user(char *filename, char *user_key, char *user_name);
FRESULT write_entry(char *filename, char *entry);
FRESULT check_for_dir(char *filename);
void get_time_from_rtc(char *rtc_lecture);
void send_buffer_via_uart(uint16_t *buffer, size_t size);


#endif // SD_FUNCTIONS_H
