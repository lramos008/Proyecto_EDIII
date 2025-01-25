# -*- coding: utf-8 -*-
"""
Created on Fri Jan 17 13:22:00 2025

@author: dramos
"""
from procesador import leer_bin_a_numpy, process_voice, normalize_array
from template_generator import generate_template
import numpy as np
import os

#Defino threshold de distancia
distance_threshold = 2.0
num_of_test_files = 30

#Genero el template
template_folder = "template"
template = generate_template(template_folder)
template.tofile("Leonardo.bin")


#Proceso archivos de prueba
test_folder = "voz"
distancias = []
max_distancias = [] 
for i in range(num_of_test_files):
    #Proceso features de la voz actual
    current_path = os.path.join(test_folder, f'feature_{i}.bin')
    current_test_voice = leer_bin_a_numpy(current_path)
    current_test_feature = process_voice(current_test_voice)
    
    # if i== 0:
    #     current_test_feature.tofile("test_file_false.bin")
    
    #Calculo las distancias euclideanas de los distintos bloques
    distancia = np.linalg.norm(template - current_test_feature, axis=1)
    #Calculo la distancia euclideana maxima obtenida para esta voz
    max_distancia = np.max(distancia)
    
    #Guardo distancias y distancias maximas en listas
    distancias.append(distancia)
    max_distancias.append(max_distancia)
    
    #Defino threshold tentativo
    threshold_tentativo = np.mean(max_distancias)



# #Calculo la distancia euclideana para cada threshold
# distancias = np.linalg.norm(template - processed_test, axis=1)

# # Verificar cuáles cumplen con el threshold
# cumplen_threshold = distancias < distance_threshold
# cantidad_que_cumplen = np.sum(cumplen_threshold)
# ratio = cantidad_que_cumplen / len(cumplen_threshold)