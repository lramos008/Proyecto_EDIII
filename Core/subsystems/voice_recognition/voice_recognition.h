#ifndef VOICE_RECOGNITION_H
#define VOICE_RECOGNITION_H
#include <stdbool.h>
/*==================[Public function prototypes]=====================*/
bool recognize_user_voice(char *template_path, char *user_name);
bool compare_features(char *feature_1, char *feature_2);
#endif // VOICE_RECOGNITION_H
