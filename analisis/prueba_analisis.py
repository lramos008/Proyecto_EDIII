# -*- coding: utf-8 -*-
"""
Created on Tue Jan 14 17:26:08 2025

@author: dramos
"""

import cmsisdsp as dsp
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

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

def ventana_hamming(N):
  """Crea una ventana de Hamming de longitud N.

  Args:
    N: Longitud de la ventana.

  Returns:
    Un array NumPy con los valores de la ventana de Hamming.
  """

  n = np.arange(N)
  window = 0.54 - 0.46 * np.cos(2 * np.pi * n / (N - 1))
  return window

def normalize_to_range_0_1(array):
    """Normaliza un array para que sus valores estén entre 0 y 1."""
    min_val = np.min(array)
    max_val = np.max(array)
    if max_val - min_val == 0:  # Evitar división por cero
        return np.zeros_like(array)
    return (array - min_val) / (max_val - min_val)


datos_audio = leer_bin_a_numpy('current_voice.bin')
hamming_windows = ventana_hamming(2048)

# Crear un arreglo de tiempo (eje x)
frecuencia_muestreo = 16000  # Hz
tiempo = np.arange(len(datos_audio)) / frecuencia_muestreo



nyquist = frecuencia_muestreo / 2  # Frecuencia de Nyquist
low_cutoff = 300 / nyquist  # Frecuencia de corte baja normalizada
high_cutoff = 3400 / nyquist  # Frecuencia de corte alta normalizada
num_taps = 401  # Número de coeficientes (orden + 1)

# Calcular los coeficientes del filtro FIR pasa bandas
coefficients = signal.firwin(num_taps, [low_cutoff, high_cutoff], pass_zero=False)

#Declaro coeficientes del filtro FIR
state_array = np.zeros(num_taps + 2048 - 1)
firf32 = dsp.arm_fir_instance_f32()
dsp.arm_fir_init_f32(firf32, num_taps, coefficients, state_array)		#No se agrega block size porque se infiere

#Señal con ventana
filtered = np.array([])
																													#a partir del tamaño del state array.
#Filtro la señal
for i in range(0, 24576, 2048):
    bloque = datos_audio[i:i + 2048]
    result = dsp.arm_fir_f32(firf32, bloque)
    filtered = np.append(filtered, result)

# Crear subgráficos
fig, axs = plt.subplots(2, 1, figsize=(8, 6))  # 2 filas, 1 columna

# Primer gráfico
axs[0].plot(tiempo, datos_audio, label='Original', color='blue')
axs[0].set_title('Señal Original')
axs[0].set_xlabel('Tiempo [s]')
axs[0].set_ylabel('Amplitud')
axs[0].legend()
axs[0].grid(True)

# Segundo gráfico
axs[1].plot(tiempo, filtered, label='Filtrada', color='red')
axs[1].set_title('Señal filtrada')
axs[1].set_xlabel('Tiempo [s]')
axs[1].set_ylabel('Amplitud')
axs[1].legend()
axs[1].grid(True)

# Ajustar diseño
plt.tight_layout()
plt.show()



#Hago la fft
nb = 2048
rfft32 = dsp.arm_rfft_fast_instance_f32()
status = dsp.arm_rfft_fast_init_f32(rfft32, nb)
result = dsp.arm_rfft_fast_f32(rfft32, filtered[0:nb] * hamming_windows, 0)
mag_result = dsp.arm_cmplx_mag_f32(result)
normalized_array = normalize_to_range_0_1(mag_result)


Fs = 16000
N = 2048
frequencies = np.fft.fftfreq(N, 1/Fs)[:N//2]

plt.figure(2)
plt.plot(frequencies, normalized_array)
plt.xlabel("Frecuencia [Hz])")
plt.ylabel("Amplitud")
plt.title("FFT")
plt.grid(True)

# plt.figure(1)
# plt.plot(np.arange(1024), normalized_array)
# plt.xlabel("Tiempo (s)")
# plt.ylabel("Amplitud")
# plt.title("Señal de audio")
# plt.grid(True)


# plt.plot(np.arange(1024), normalized_array2)
# plt.xlabel("Tiempo (s)")
# plt.ylabel("Amplitud")
# plt.title("Señal de audio")

# plt.show()
