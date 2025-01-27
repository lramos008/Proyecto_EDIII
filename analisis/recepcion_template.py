# -*- coding: utf-8 -*-
"""
Created on Mon Jan 27 12:30:46 2025

@author: dramos
"""
from recepcion_uart import recibir_vector_por_bloques

def recibir_m_vectores(port, baudrate, num_vectores, num_datos, tamano_bloque):
    """
    Usa `recibir_vector_por_bloques_indefinidamente` para recibir múltiples vectores desde UART.

    Args:
        port (str): Puerto serie a utilizar (ej. 'COM3').
        baudrate (int): Velocidad en baudios.
        num_vectores (int): Número total de vectores que se espera recibir.
        num_datos (int): Número total de floats en cada vector.
        tamano_bloque (int): Cantidad de floats por bloque.

    Returns:
        list[np.ndarray]: Lista con los vectores recibidos, o None si hubo un error.
    """
    vectores = []  # Lista para almacenar los vectores recibidos

    try:
        for i in range(num_vectores):
            print(f"\nRecibiendo vector {i + 1} de {num_vectores}...")

            # Usar la función existente para recibir un solo vector
            vector = recibir_vector_por_bloques(port, baudrate, num_datos, tamano_bloque)

            if vector is None:
                print(f"Error al recibir el vector {i + 1}. Cancelando recepción.")
                return None  # Detener si hay algún error
            
            vectores.append(vector)  # Guardar el vector recibido

        print("\nTodos los vectores fueron recibidos con éxito.")
        return vectores

    except Exception as e:
        print(f"Error general: {e}")
        return None
