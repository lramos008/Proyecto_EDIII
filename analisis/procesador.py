# -*- coding: utf-8 -*-
"""
Created on Thu Jan 16 15:08:35 2025

@author: dramos
"""

import cmsisdsp as dsp
import numpy as np
from scipy import signal


def leer_bin_a_numpy(ruta_archivo, dtype='float32'):
  """
  Lee un archivo binario y lo carga en un array de NumPy.

  Args:
    ruta_archivo: La ruta completa del archivo binario.
    dtype: El tipo de datos de los elementos en el archivo (por defecto, float32).

  Returns:
    Un array de NumPy con los datos del archivo.
  """

  # Abrir el archivo en modo binario
  with open(ruta_archivo, 'rb') as f:
    # Leer todos los bytes del archivo
    data = f.read()

  # Interpretar los bytes como un array de NumPy con el tipo de datos especificado
  array_numpy = np.frombuffer(data, dtype=dtype)

  return array_numpy

#%%
def ventana_hamming(N):
 """Crea una ventana de hamming de longitud N)"""
 n = np.arange(N)
 window = 0.54 - 0.46 * np.cos(2 * np.pi * n / (N - 1))
 return window

#%%
def normalize_array(array):
    """Normaliza un array para que sus valores estén entre 0 y 1."""
    min_val = np.min(array)
    max_val = np.max(array)
    if max_val - min_val == 0:  # Evitar división por cero
        return np.zeros_like(array)
    return (array - min_val) / (max_val - min_val)

#%%
def filter_signal(data, coefficients, block_size):
    num_of_taps = coefficients.size
    #Declaro state array. Con este array el filtro FIR conoce la historia previa a medida que avanza
    state_array = np.zeros(num_of_taps + block_size - 1)
    #Declaro instancia de filtro fir e inicializo
    firf32 = dsp.arm_fir_instance_f32()
    dsp.arm_fir_init_f32(firf32, num_of_taps, coefficients, state_array)		#No se agrega block size porque se infiere
    
    #Declaro array donde se guardara la señal filtrada
    filtered_signal = np.array([])
    
    #Filtro la señal de a bloques
    for i in range(0, data.size, block_size):
        block = data[i:i + 2048]                                                #Tomo bloque de 2048 elementos de la señal
        result = dsp.arm_fir_f32(firf32, block)
        filtered_signal = np.append(filtered_signal, result)
    
    return filtered_signal

#%%
def get_fft_mag(data, block_size):
    #Defino variables
    #energy_threshold = 1.0
    overlap = block_size // 2
    num_blocks = (data.size - overlap) // (block_size - overlap)
    hamming_window = ventana_hamming(block_size)
    
    #Declaro instancia de rfft y la inicializo
    rfft32 = dsp.arm_rfft_fast_instance_f32()
    status = dsp.arm_rfft_fast_init_f32(rfft32, block_size)
    
    #Declaro lista para guardar el resultado de la fft
    result = []
    energies = []
    
    #Proceso cada bloque
    for i in range(num_blocks):
        #Se define inicio y final de cada bloque
        start_index = i * (block_size - overlap)
        end_index = start_index + block_size
        
        #Extraigo bloque de señal
        block = data[start_index:end_index]
        
        #Aplico fft al bloque
        fft_block = dsp.arm_rfft_fast_f32(rfft32, block * hamming_window, 0)
        
        #Calculo magnitud
        fft_mag = dsp.arm_cmplx_mag_f32(fft_block) / block_size
        
        #Calculo la energia del bloque
        energy = dsp.arm_dot_prod_f32(fft_mag, fft_mag, len(fft_mag))
        
        #Normalizo
        normalized_mag = normalize_array(fft_mag)
        
        #Almaceno el resultado
        result.append(normalized_mag)
        energies.append(energy)
        
    #Convierto fft_result en un array de numpy
    result = np.array(result)
    energies = np.array(energies)
    
    return result, energies

#%%
def process_voice(data):
    Fs = 16000
    nyquist = Fs / 2
    low_cutoff = 300 / nyquist  # Frecuencia de corte baja normalizada
    high_cutoff = 3400 / nyquist  # Frecuencia de corte alta normalizada
    num_of_taps = 401  # Número de coeficientes (orden + 1)
    block_size = 2048
    frequencies = np.fft.fftfreq(block_size, 1/Fs)[:block_size//2]
    
    #Calculo los coeficientes del filtro fir pasa bandas
    coefficients = signal.firwin(num_of_taps, [low_cutoff, high_cutoff], pass_zero=False)
    
    #Filtro la señal
    filtered_signal = filter_signal(data, coefficients, block_size)
    
    #Calculo la magnitud normalizada de la fft
    fft_mag, energies = get_fft_mag(filtered_signal, block_size)
    return fft_mag, energies














