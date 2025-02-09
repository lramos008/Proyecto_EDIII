#include "common_utils.h"

void lock_task(void *pvParameters){
	bool lock_state;
	HAL_GPIO_WritePin(Lock_GPIO_Port, Lock_Pin, GPIO_PIN_SET);
	while(1){
		xQueueReceive(lock_state_queue, &lock_state, portMAX_DELAY);
		if(lock_state){
			HAL_GPIO_WritePin(Lock_GPIO_Port, Lock_Pin, GPIO_PIN_RESET);			//Abro la cerradura
			HAL_Delay(3000);														//Doy tiempo prudencial para pasar
			HAL_GPIO_WritePin(Lock_GPIO_Port, Lock_Pin, GPIO_PIN_SET);				//Cierro la cerradura
		}
	}
}
