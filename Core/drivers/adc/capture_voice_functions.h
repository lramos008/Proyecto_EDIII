#ifndef CAPTURE_VOICE_FUNCTIONS_H
#define CAPTURE_VOICE_FUNCTIONS_H
/*================[Public functions]=====================*/
void store_voice(uint16_t *voice_buffer, uint32_t size, char *filename);
void capture_voice(uint16_t *buffer, uint32_t size);
void extract_and_save_features(char *voice_name, char *feature_name);
void generate_template(void);
#endif /*CAPTURE_VOICE_FUNCTIONS_H */
