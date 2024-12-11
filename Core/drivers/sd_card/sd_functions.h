#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H
#include "ff.h"
#include <stdbool.h>
/*===============[Public functions]=====================*/
void send_uart(char *string);
void mount_sd(const char* path);
void unmount_sd(const char* path);
FRESULT check_if_file_exists(char *filename);
FRESULT create_file(char *filename, char *header);
FRESULT look_for_user(char *filename, char *username, uint16_t sequence);
FRESULT save_buffer_on_sd(char *filename, uint16_t *buffer, uint16_t buf_size);
void read_voice_buffer(char *filename, uint16_t *buffer, uint16_t length);
void read_template(char *filename, float *buffer, uint16_t length);
FRESULT write_entry(char *filename, char *entry);
void get_time_from_rtc(char *rtc_lecture);
void send_buffer_via_uart(uint16_t *buffer, size_t size);
void format_sd(const char *path);

#endif // SD_FUNCTIONS_H
