# -*- coding: utf-8 -*-
"""
Created on Mon Jan 27 12:10:15 2025

@author: dramos
"""

import serial
import struct
import numpy as np
import keyboard

def recibir_vector_completo_uint16_t(port, baudrate, num_datos):
    """
    Recibe un vector completo de uint16_t de una sola vez desde UART, esperando indefinidamente.

    Args:
        port (str): Puerto serie a utilizar (ej. 'COM3').
        baudrate (int): Velocidad en baudios.
        num_datos (int): Número total de uint16_t a recibir.

    Returns:
        np.ndarray: El vector completo de uint16_t recibido, o None si hubo algún error.
    """
    try:
        # Abrir la conexión serie
        with serial.Serial(port, baudrate, timeout=None) as ser:  # timeout=None: espera indefinida
            print(f"Esperando {num_datos} uint16_t (total {num_datos * 2} bytes)...")

            # Calcular la cantidad total de bytes esperados
            bytes_totales = num_datos * 2  # Cada uint16_t ocupa 2 bytes

            # Leer todos los bytes esperados
            datos_crudos = ser.read(bytes_totales)

            # Verificar que se hayan recibido todos los bytes
            if len(datos_crudos) != bytes_totales:
                print(f"Error: Se recibieron {len(datos_crudos)} bytes, pero se esperaban {bytes_totales}.")
                return None

            # Desempaquetar los bytes en un array de uint16_t
            formato = f'<{num_datos}H'  # '<' para little-endian, 'H' para uint16_t
            vector = struct.unpack(formato, datos_crudos)

            # Convertir a numpy array
            return np.array(vector, dtype=np.uint16)

    except serial.SerialException as e:
        print(f"Error en la comunicación serie: {e}")
        return None
    except struct.error as e:
        print(f"Error al desempaquetar los datos: {e}")
        return None




def recibir_n_vectores_uint16_t(port, baudrate, num_datos_por_vector, num_vectores):
    """
    Recibe N vectores completos de uint16_t desde UART, con la opción de cancelar la recepción 
    presionando la tecla Esc.

    Args:
        port (str): Puerto serie a utilizar (ej. 'COM3').
        baudrate (int): Velocidad en baudios.
        num_datos_por_vector (int): Número total de uint16_t por vector.
        num_vectores (int): Número total de vectores a recibir.

    Returns:
        list[np.ndarray]: Lista de vectores (cada uno es un np.ndarray de uint16_t),
                          o None si se cancela la recepción con Esc.
    """
    vectores = []

    for i in range(num_vectores):
        if keyboard.is_pressed("esc"):
            print("Recepción cancelada por el usuario (tecla Esc presionada).")
            return None

        print(f"Recibiendo el vector {i+1} de {num_vectores}...")
        vector = recibir_vector_completo_uint16_t(port, baudrate, num_datos_por_vector)

        if vector is None:
            print(f"Error al recibir el vector {i+1}. Cancelando la recepción.")
            return None

        vectores.append(vector)

    return vectores

