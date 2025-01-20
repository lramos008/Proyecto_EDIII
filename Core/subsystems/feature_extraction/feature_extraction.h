#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
/*===================[Public function prototypes]=========================*/
bool filter_signal(char *input_file, char *output_file);
bool get_fft_feature(char *input_file, char *output_file);
#endif // FEATURE_EXTRACTION_H
