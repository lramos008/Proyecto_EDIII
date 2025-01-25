# -*- coding: utf-8 -*-
"""
Created on Fri Jan 17 17:26:53 2025

@author: dramos
"""

from procesador import leer_bin_a_numpy, process_voice, normalize_array
from template_generator import generate_template
import numpy as np
import os

#Defino threshold de distancia
distance_threshold = 4.7
num_of_test_files = 10

#Genero el template
template_folder = "template"
template = generate_template(template_folder)

#Proceso archivos de prueba
test_folder = "voz"

ratios = []
estado = [] 
distancias = []
voces_reconocidas = 0
for i in range(num_of_test_files):
    voz = f'feature_{i}.bin'
    current_path = os.path.join(test_folder, voz)
    current_test_voice = leer_bin_a_numpy(current_path)
    current_test_feature = process_voice(current_test_voice)

    #Calculo las distancias euclideanas de los distintos bloques
    distancia = np.linalg.norm(template - current_test_feature, axis=1)
    #Verifico que distancias cumplen
    distancias_ok = distancia < distance_threshold

    #Calculo el ratio
    ratio = np.sum(distancias_ok) / len(distancias_ok)
    
    #Guardo
    ratios.append(ratio)
    estado.append(distancias_ok)
    distancias.append(distancia)
    if(np.sum(distancias_ok) >= 21):
        print(f'Voz {i} reconocida.\n')
        voces_reconocidas += 1
    else:
        print(f'Voz {i} no reconocida.\n')
        
print(f'Voces reconocidas: {voces_reconocidas} / {num_of_test_files}\n')