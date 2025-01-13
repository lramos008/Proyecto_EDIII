#ifndef USER_AND_ENTRY_H
#define USER_AND_ENTRY_H
#include "common_utils.h"

/*===========================[Public function prototypes]=============================*/
bool check_if_file_exists(char *filename);
bool create_file(char *filename, char *header);
void get_time_from_rtc(char *rtc_lecture);
bool write_entry (char *filename, char *entry);
bool search_user(char *filename, char *user_key, char *user_name);
#endif // USER_AND_ENTRY_H
