# -*- coding: utf-8 -*-
"""
Created on Thu Jan 16 17:22:07 2025

@author: dramos
"""

from procesador import leer_bin_a_numpy, process_voice, normalize_array
import os
import numpy as np


# def generate_template(folder):
#     """Genera un template normalizado a partir de un conjunto de 10 voces"""
#     num_of_files = 10
#     features = []
#     for i in range(num_of_files):
#         current_file = f'feature_{i}.bin'
#         path = os.path.join(folder, current_file)                   #Armo la direccion del archivo
#         current_voice = leer_bin_a_numpy(path)                      #Leo voz desde archivo
#         fft_mag_norm = process_voice(current_voice)                 #Proceso para obtener los features
#         features.append(fft_mag_norm)                               #Agrego los features a esta lista
        
#     resul = np.sum(features, axis = 0) / len(features)              #Se tienen 10 elementos de dimension (23, 1024)
#     norm_result = normalize_array(resul)                            #Promedio punto a punto y normalizo
#     return norm_result


def generate_template(señales_de_voz):
    features = []
    #Proceso cada señal de voz
    for señal in señales_de_voz:
        fft_mag_norm = process_voice(señal)                         #Proceso cada voz para obtener los features
        features.append(fft_mag_norm)
    
    resul = np.sum(features, axis = 0) / len(features)              #Promedio punto a punto

    norm_resul = normalize_array(resul)
    return norm_resul
        
    




