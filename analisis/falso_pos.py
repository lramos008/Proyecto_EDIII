# -*- coding: utf-8 -*-
"""
Created on Fri Jan 17 15:49:12 2025

@author: dramos
"""

from procesador import leer_bin_a_numpy, process_voice, normalize_array
from template_generator import generate_template
import numpy as np
import os

#Genero el template
template_folder = "conjunto_template"
template = generate_template(template_folder)

current_test_voice = leer_bin_a_numpy("current_voice.bin")
current_test_feature = process_voice(current_test_voice)

#Calculo las distancias euclideanas de los distintos bloques
distancia = np.linalg.norm(template - current_test_feature, axis=1)
#Calculo la distancia euclideana maxima obtenida para esta voz
max_distancia = np.max(distancia)