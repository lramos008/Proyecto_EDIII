#ifndef SD_UTILS_H
#define SD_UTILS_H
#include "sd_functions.h"
#include "utils.h"

/*==========================[Public functions prototypes]===================================*/
bool initialize_sd_and_verify_files(indicatorMessage *error_message);
bool process_user_key(char *user_key, char *user_name, indicatorMessage *error_message);
void build_entry_message(char *entry, char *user_name, const char *status);
bool recognize_user_voice(char *template_path, char *user_name, indicatorMessage *message);

#endif //SD_UTILS_H
