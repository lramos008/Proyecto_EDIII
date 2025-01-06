#ifndef PROCESSING_FUNCTIONS_H
#define PROCESSING_FUNCTIONS_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
/*===============[Public function prototypes]================*/
bool filter_and_windowing(float32_t *psrc, float32_t *pdst, uint32_t size);
bool get_normalized_fft_magnitude(float32_t *psrc, float32_t *pdst, uint32_t size);
#endif // PROCESSING_FUNCTIONS_H
