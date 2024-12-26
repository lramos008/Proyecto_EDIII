#ifndef PROCESSING_H
#define PROCESSING_H
#include "common_utils.h"
#define ARM_MATH_CM4
#include "arm_math.h"
/*=========================[Public defines]==============================*/

/*================[Public function prototypes]=====================*/
void process_frame(float32_t *psrc, float32_t *pdst, uint32_t size);
bool compare_features(float32_t *template, float32_t *target, uint32_t size);
#endif // PROCESSING_H
