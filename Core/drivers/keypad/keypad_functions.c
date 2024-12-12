#include "utils.h"
/*================[Private defines]=======================*/
#define ROW_LENGTH 4
#define COL_LENGTH 4
#define GPIO_ROW GPIOC
#define GPIO_COL GPIOC

/*================[Private data types]=====================*/
typedef enum{
	ROW_1 = GPIO_PIN_6,
	ROW_2 = GPIO_PIN_7,
	ROW_3 = GPIO_PIN_8,
	ROW_4 = GPIO_PIN_9
}row_t;

typedef enum{
	COL_1 = GPIO_PIN_0,
	COL_2 = GPIO_PIN_1,
	COL_3 = GPIO_PIN_2,
	COL_4 = GPIO_PIN_3
}col_t;

typedef enum{
	IDLE_COL = 0,
	ACTIVE_COL
}col_state;

/*================[Private variables]=====================*/
row_t keypad_rows[ROW_LENGTH] = {ROW_1, ROW_2, ROW_3, ROW_4};
col_t keypad_cols[COL_LENGTH] = {COL_1, COL_2, COL_3, COL_4};
char keypad_chars[ROW_LENGTH][COL_LENGTH] = { {'1', '2', '3', 'A'},
	      	  	  	  	  		  	  	  	  {'4', '5', '6', 'B'},
											  {'7', '8', '9', 'C'},
											  {'*', '0', '#', 'D'} };

/*================[Private functions]====================*/
static void set_row(row_t row){
	HAL_GPIO_WritePin(GPIO_ROW, row, GPIO_PIN_SET);
	return;
}

static void reset_row(row_t row){
	HAL_GPIO_WritePin(GPIO_ROW, row, GPIO_PIN_RESET);
	return;
}

static GPIO_PinState read_col(col_t col){
	return HAL_GPIO_ReadPin(GPIO_COL, col);
}

/*================[Public functions]=====================*/
char read_keypad(void){
	/* La funcion barre las filas del keypad, activando una por una individualmente y leyendo la columna asociada,
	 * para así poder triangular la tecla presionada. Se realiza doble confirmacion de tecla presionada.
	 */
	char key_pressed;
	for(uint8_t i = 0; i < ROW_LENGTH; i++){
		set_row(keypad_rows[i]);
		for(uint8_t j = 0; j < COL_LENGTH; j++){
			if(read_col(keypad_cols[j]) == GPIO_PIN_SET){					//Verifico si alguna columna esta en alto (tecla presionada)
				vTaskDelay(20 / portTICK_RATE_MS);							//Si una de las columnas dio alto, espero 20ms y verifico que siga en alto para confirmar
				if(read_col(keypad_cols[j]) == GPIO_PIN_SET){
					key_pressed = keypad_chars[i][j];						//Guardo la tecla presionada
					while(read_col(keypad_cols[j]) == GPIO_PIN_SET);		//Espero a que se deje de apretar la tecla
					reset_row(keypad_rows[i]);								//Reseteo la fila en alto antes de salir de la funcion
					vTaskDelay(10/portTICK_RATE_MS);						//Pequeño delay para asegurar que el boton fue liberado completamente
					return key_pressed;
				}
			}
		}
		reset_row(keypad_rows[i]);
	}
	return 0;
}
