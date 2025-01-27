#ifndef VOICE_CAPTURE_H
#define VOICE_CAPTURE_H
#include <stdbool.h>
/*==============[Public function prototypes]================*/
bool capture_voice_signal(char *voice_path);
void capture_voice(uint16_t *buffer, uint32_t size);
#endif // VOICE_CAPTURE_H
