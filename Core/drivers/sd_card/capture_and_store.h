#ifndef CAPTURE_AND_STORE_H
#define CAPTURE_AND_STORE_H
#include "common_utils.h"
void capture_voice(uint16_t *buffer, uint32_t size);
void store_voice(uint16_t *voice_buffer, uint32_t buf_size, uint32_t frame_size, char *filename);
void extract_and_save_features(char *voice_name, char *feature_name);
bool check_voice(char *template_path, char *feature_path);
#endif //CAPTURE_AND_STORE_H
