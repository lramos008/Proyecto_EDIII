#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H
#include "common_utils.h"
void receive_user_key(char *user_key);
bool is_template_creation(char *user_key);
void handle_template_creation(void);
void handle_user_verification(char *user_key);
#endif //ACCESS_CONTROL_H
