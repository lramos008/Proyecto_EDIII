#ifndef BLOCK_PROCESSING_H
#define BLOCK_PROCESSING_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
/*===============[Public function prototypes]================*/
bool process_block(float32_t *psrc, float32_t *pdst, uint32_t size);
#endif // BLOCK_PROCESSING_H
