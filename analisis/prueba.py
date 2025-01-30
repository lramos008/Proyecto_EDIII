# -*- coding: utf-8 -*-
"""
Created on Tue Jan 28 16:00:07 2025

@author: dramos
"""

from recepcion_uart import recibir_n_vectores_uint16_t
from template_generator import generate_template
from procesador import process_voice
import numpy as np
import argparse
import os


#Configuraciones para conversion a tension
VCC = 3.3
ADC_RES = 4096

#Direccion de guardado de voces y template
carpeta_voces = "voces"
carpeta_template = "mis_templates"
nombre_archivo = "Silencio"
template_path = os.path.join(carpeta_template, nombre_archivo + ".bin")

#Chequeo si existen las carpetas donde guardar las voces y el template. Sino las creo
os.makedirs(carpeta_voces, exist_ok=True)
os.makedirs(carpeta_template, exist_ok=True)
os.makedirs(nombre_archivo, exist_ok=True)

# #Recibo señales de voz desde el stm32 nucleo
PORT = 'COM18'
BAUDRATE = 115200
NUM_DATOS = 24576
NUM_VECTORES = 20
recepcion = recibir_n_vectores_uint16_t(
       PORT, BAUDRATE, NUM_DATOS, NUM_VECTORES)

#Convierto a tensiones las señales
señales_de_voz = [(arr * VCC / ADC_RES).astype(np.float32) for arr in recepcion]
  
#Guardo las señales en archivos
for i, array in enumerate(señales_de_voz):
     path = os.path.join(nombre_archivo, f'voz_{i + 1}.bin')
     array.tofile(path)
print(f"Voces guardadas en {nombre_archivo}\n")

# template = generate_template(señales_de_voz[0 : NUM_VECTORES // 2])

# template = template.astype(np.float32)
# template.tofile(nombre_archivo + ".bin")
#  #Guardo el template
# template.tofile(template_path)
# print(f'Template guardado en {template_path}.\n')

# promedio = np.mean(mediciones, axis = 0)
# maximo = np.max(promedio)


# template = generate_template(señales_de_voz[0 : NUM_VECTORES // 2])
