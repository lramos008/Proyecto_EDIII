#ifndef DSP_FUNCTIONS_H
#define DSP_FUNCTIONS_H
/*=========================[Public includes]=============================*/
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "cmsis_os.h"
#include "main.h"
/*=========================[Public defines]==============================*/
#define BUFFER_SIZE 20480
#define FRAME_SIZE 2048
#define MAG_SIZE (FRAME_SIZE/2)
#define CHUNK_SIZE (BUFFER_SIZE / FRAME_SIZE)
/*=========================[Public functions]=============================*/
void process_signal(float32_t *psrc, float32_t *pdst, size_t block_size);
uint8_t compare_features(float32_t *feature_1, float32_t *feature_2, size_t length);
#endif /* DSP_FUNCTIONS_H */
