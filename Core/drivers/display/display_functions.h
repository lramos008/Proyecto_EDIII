#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H
#include "main.h"
#include "cmsis_os.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include <string.h>
#include <stdio.h>

/*================[Public function prototypes]=====================*/
void display_init(void);
void display_start_msg(void);
void display_sequence_entry_msg(uint8_t input_cont);
void display_incomplete_entry_msg(void);
void display_timeout_msg(void);
void display_access_granted_msg(void);
void display_user_found_msg(void);
void display_user_not_found_msg(void);
void display_start_voice_recognition_msg(void);
void countdown_msg(void);
void display_capturing_voice_msg(void);
void display_recognized_voice_msg(void);
void display_not_recognized_voice_msg(void);
#endif /* DISPLAY_FUNCTIONS_H */
