#ifndef COMPARE_FEATURES_H
#define COMPARE_FEATURES_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
bool compare_features(float32_t *feature_1, float32_t *feature_2, uint32_t size);
#endif //COMPARE_FEATURES_H
