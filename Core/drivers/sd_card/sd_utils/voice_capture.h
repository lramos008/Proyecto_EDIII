#ifndef VOICE_CAPTURE_H
#define VOICE_CAPTURE_H

/*===========[Public function prototypes]===============*/
void capture_voice(uint16_t *buffer, uint32_t size);
bool store_voice(uint16_t *voice_buffer, uint32_t buf_size, uint32_t frame_size, char *filename);
bool extract_and_save_features(char *voice_name, char *feature_name);
bool check_voice(char *template_path, char *feature_path);
#endif // VOICE_CAPTURE_H
