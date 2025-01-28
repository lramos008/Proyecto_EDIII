# -*- coding: utf-8 -*-
"""
Created on Tue Jan 28 14:34:40 2025

@author: dramos
"""

from recepcion_uart import recibir_n_vectores_uint16_t
from template_generator import generate_template
import numpy as np
import argparse
import os

def main():
    #Creo objeto parser
    parser = argparse.ArgumentParser(description="Generador de template")

    #Defino argumentos que acepta el script
    parser.add_argument("--COM", type=int, help="Número de puerto COM.")
    parser.add_argument("--baudrate", type=int, default=115200,
                        help="Velocidad de transmision UART.")
    parser.add_argument("--filename", type=str,
                        default="template.bin", help="Nombre del archivo.")
    
    #Parseo los argumentos
    args = parser.parse_args()
    
    #Configuraciones para conversion a tension
    VCC = 3.3
    ADC_RES = 4096
    
    #Direccion de guardado de voces y template
    carpeta_voces = args.filename
    carpeta_template = args.template_folder
    nombre_archivo = args.filename
    template_path = os.path.join(carpeta_template, nombre_archivo + ".bin")
    
    #Chequeo si existen las carpetas donde guardar las voces y el template. Sino las creo
    os.makedirs(carpeta_voces, exist_ok=True)
    os.makedirs(carpeta_template, exist_ok=True)
    
    #Recibo señales de voz desde el stm32 nucleo
    PORT = f'COM{args.COM}'
    BAUDRATE = args.baudrate
    NUM_DATOS = 24576
    NUM_VECTORES = 20
    recepcion = recibir_n_vectores_uint16_t(
          PORT, BAUDRATE, NUM_DATOS, NUM_VECTORES)
    
    #Convierto a tensiones las señales
    señales_de_voz = [(arr * VCC / ADC_RES) for arr in recepcion]
  
    #Guardo las señales en archivos
    for i, array in enumerate(señales_de_voz):
         path = os.path.join(carpeta_voces, f'voz_{i + 1}.bin')
         array.tofile(path)
    print(f"Voces guardadas en {nombre_archivo}\n")
  
    #Genero template
    template = generate_template(señales_de_voz[0 : NUM_VECTORES // 2])
  
     #Guardo el template
    template.tofile(template_path)
    print(f'Template guardado en {template_path}.\n')

# Ejecutar la función main
if __name__ == "__main__":
    main()