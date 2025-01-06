#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"



static bool calculate_euclidean_distance(float32_t *array_1, float32_t *array_2, float32_t *result,  uint32_t size){
	float32_t *diff = NULL;
	float32_t distance;
	arm_status status;

	//Reservo memoria para los vectores de procesamiento
	diff = pvPortMalloc(FLOAT_SIZE_BYTES(size));
	if(diff == NULL){
		return false;
	}

	//Resto vectores
	arm_sub_f32(array_1, array_2, diff, size);

	//Calculo la suma de los cuadrados
	arm_dot_prod_f32(diff, diff, size, &distance);

	//Calculo la raiz para obtener la distancia euclidea
	status = arm_sqrt_f32(distance, &distance);
	if(status != ARM_MATH_SUCCESS){
		vPortFree(diff);
		return false;
	}

	//Guardo la distancia en result
	*result = distance;

	//Libero memoria y devuelvo la distancia euclidea
	vPortFree(diff);
	return true;											//Operacion exitosa
}

bool compare_features(float32_t *feature_1, float32_t *feature_2, uint32_t size){
	//Declaracion de variables
	float32_t distance;

	//Calculo distancia euclideana entre los vectores
	if(!calculate_euclidean_distance(feature_1, feature_2, &distance, size)){
		//Devuelve false si hubo error al asignar memoria o hacer la operacion
		return false;
	}

	//Se devuelve true si los features son similares, de acuerdo al threshold permitido
	return (distance <= DISTANCE_THRESHOLD) ? true : false;
}

//bool compare_features(float32_t *template, float32_t *target, uint32_t size){
//	display_message_t message;
//	uint32_t accepted_bins = 0;										//Bins dentro del threshold
//	float32_t accepted_bins_ratio;									//Proporcion de bins aceptados con respecto al total
//	bool comp_state;												//Estado de comparacion de los features
//
//	//Reservo memoria para los vectores de procesamiento
//	float32_t *diff = pvPortMalloc(FLOAT_SIZE_BYTES(size));
//	float32_t *abs_diff = pvPortMalloc(FLOAT_SIZE_BYTES(size));
//	if(diff == NULL || abs_diff == NULL){
//		message = DISPLAY_ERROR_MEMORY;
//		xQueueSend(display_queue, &message, portMAX_DELAY);
//		while(1);
//	}
//
//	//Resto bin a bin los features y calculo el valor absoluto
//	arm_sub_f32(template, target, diff, size);
//	arm_abs_f32(diff, abs_diff, size);
//
//	//Cuento cuantos bins estan por debajo del threshold
//	for(uint32_t i = 0; i < size; i++){
//		if(abs_diff[i] < DIF_THRESHOLD){
//			accepted_bins++;
//		}
//	}
//
//	/*Libero la memoria utilizada*/
//	vPortFree(diff);
//	vPortFree(abs_diff);
//
//	//Calculo el ratio de bins aceptados
//	accepted_bins_ratio = ((float32_t) accepted_bins) / size;
//
//	//Verifico si el ratio de bins aceptados es mayor al threshold BIN_RATIO_THRESHOLD
//	comp_state = (accepted_bins_ratio >= BIN_RATIO_THRESHOLD) ? true : false;
//	return comp_state;
//}
