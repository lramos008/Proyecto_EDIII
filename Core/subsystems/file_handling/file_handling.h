#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H
#include "common_utils.h"
bool initialize_sd_and_verify_files(display_message_t *error_message);
bool process_user_key(char *user_key, char *user_name, display_message_t *error_message);
void build_entry_message(char *entry, char *user_name, const char *status);
#endif //  FILE_HANDLING_H
