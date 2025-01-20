#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

float32_t euclidean_distance(float32_t *array_1, float32_t *array_2){
	float32_t diff[FEATURE_SIZE] = {0};
	float32_t distance = 0;

	//Resto vectores
	arm_sub_f32(array_1, array_2, diff, FEATURE_SIZE);

	//Calculo la suma de los cuadrados
	arm_dot_prod_f32(diff, diff, FEATURE_SIZE, &distance);

	//Calculo la raiz para obtener la distancia euclideana
	arm_sqrt_f32(distance, &distance);

	//Devuelvo distancia calculada
	return distance;
}
