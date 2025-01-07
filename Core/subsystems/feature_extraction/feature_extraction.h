#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
/*===================[Public function prototypes]=========================*/
bool extract_features_from_file(char *target_file, char *feature_file, uint32_t total_size,  uint32_t block_size, float32_t overlap_ratio);
#endif // FEATURE_EXTRACTION_H
