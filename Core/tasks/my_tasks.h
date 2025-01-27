#ifndef TASKS_H
#define TASKS_H
/*================[Public task prototype]=====================*/
void display_task(void *pvParameters);
void keypad_task(void *pvParameters);
void sd_task(void *pvParameters);
void lock_task(void *pvParameters);
#endif /* TASKS_H */
