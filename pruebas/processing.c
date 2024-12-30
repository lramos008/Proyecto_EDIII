#define __FPU_PRESENT  1U
#define ARM_MATH_CM4
#include "arm_math.h"
#include "common_utils.h"

#define SAMPLE_RATE 16000                       //Frecuencia de muestreo 16 kHz
#define BLOCK_SIZE 1024                         //Tamaño del bloque en muestras
#define FFT_SIZE 1024                           //Tamaño de la FFT (potencia de 2)
#define WINDOWS_SIZE BLOCK_SIZE
#define AUDIO_SIZE 48128                        //Tamaño total de la señal de audio (en muestras, 1.5 seg)

float32_t hamming_window[WINDOWS_SIZE];

//Funcion para inicializar la ventana de hamming
void init_hamming_window(){
    for(uint8_t i = 0; i < WINDOWS_SIZE; i++){
        hamming_window[i] = 0.54 - 0.46 * arm_cos_f32((2.0f * PI * i) / (size - 1));
    }
}