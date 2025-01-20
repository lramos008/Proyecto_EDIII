#ifndef PROCESSING_FUNCTIONS_H
#define PROCESSING_FUNCTIONS_H
#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"
/*===============[Public function prototypes]================*/
void initialize_filter(arm_fir_instance_f32 *fir_instance, float32_t *state, uint32_t block_size);
void filter_block(arm_fir_instance_f32 *fir_instance, float32_t *psrc, float32_t *pdst, uint32_t block_size);
void apply_hamming_windowing(float32_t *psrc, uint32_t block_size);
bool initialize_fft(arm_rfft_fast_instance_f32 *rfft_instance, uint32_t block_size);
void calculate_rfft(arm_rfft_fast_instance_f32 *rfft_instance, float32_t *psrc, float32_t *pdst);
void calculate_magnitude(float32_t *psrc, float32_t *pdst, uint32_t block_size);
void normalize_array(float32_t *psrc, uint32_t block_size);
#endif // PROCESSING_FUNCTIONS_H
