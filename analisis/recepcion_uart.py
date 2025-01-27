# -*- coding: utf-8 -*-
"""
Created on Mon Jan 27 12:10:15 2025

@author: dramos
"""

import serial
import struct
import numpy as np

def recibir_vector_uint16_t_bloques(port, baudrate, num_datos, tamano_bloque):
    """
    Recibe un vector de uint16_t en bloques por UART, esperando indefinidamente hasta completar todos los datos.

    Args:
        port (str): Puerto serie a utilizar (ej. 'COM3').
        baudrate (int): Velocidad en baudios.
        num_datos (int): Número total de uint16_t a recibir.
        tamano_bloque (int): Número de uint16_t por bloque a recibir.

    Returns:
        np.ndarray: El vector completo de uint16_t recibido, o None si hubo algún error.
    """
    try:
        # Abrir la conexión serie
        with serial.Serial(port, baudrate) as ser:
            print(f"Esperando {num_datos} uint16_t en bloques de {tamano_bloque}...")

            total_recibidos = 0
            vector_completo = []

            # Mientras no recibamos todos los datos
            while total_recibidos < num_datos:
                # Calcular el tamaño del bloque que falta
                cantidad_faltante = num_datos - total_recibidos
                tamano_bloque_actual = min(cantidad_faltante, tamano_bloque)

                # Leer el bloque de datos
                datos_bloque = ser.read(tamano_bloque_actual * 2)  # Cada uint16_t son 2 bytes

                if len(datos_bloque) != tamano_bloque_actual * 2:
                    print(f"Error: Se recibieron {len(datos_bloque)} bytes, pero se esperaban {tamano_bloque_actual * 2}.")
                    return None

                # Desempaquetar los datos recibidos (uint16_t)
                bloque = struct.unpack(f'<{tamano_bloque_actual}H', datos_bloque)

                # Agregar el bloque al vector completo
                vector_completo.extend(bloque)
                total_recibidos += tamano_bloque_actual

                print(f"Recibido bloque {total_recibidos} de {num_datos}.")

            return vector_completo  # Devolver el vector completo como lista de uint16_t

    except serial.SerialException as e:
        print(f"Error en la comunicación serie: {e}")
        return None
    except struct.error as e:
        print(f"Error al desempaquetar los datos: {e}")
        return None


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




def recibir_multiples_vectores_uint16_t(port, baudrate, num_vectores, tamano_vector, tamano_bloque):
    """
    Recibe múltiples vectores de uint16_t desde UART.

    Args:
        port (str): Puerto serie a utilizar (ej. 'COM3').
        baudrate (int): Velocidad en baudios.
        num_vectores (int): Número total de vectores a recibir.
        tamano_vector (int): Tamaño (número de uint16_t) de cada vector.
        tamano_bloque (int): Número de uint16_t por bloque a recibir.

    Returns:
        list[np.ndarray]: Lista de vectores uint16_t recibidos, o None si hubo algún error.
    """
    try:
        # Abrir la conexión serie una vez
        with serial.Serial(port, baudrate) as ser:
            vectores_recibidos = []

            print(f"Esperando {num_vectores} vectores de tamaño {tamano_vector}...")

            # Recibir cada vector utilizando la función existente
            for i in range(num_vectores):
                print(f"\nRecibiendo vector {i + 1} de {num_vectores}...")
                vector = recibir_vector_uint16_t_bloques(
                    port=port,
                    baudrate=baudrate,
                    num_datos=tamano_vector,
                    tamano_bloque=tamano_bloque
                )

                # Verificar si hubo un error al recibir el vector
                if vector is None:
                    print(f"Error al recibir el vector {i + 1}.")
                    return None

                # Agregar el vector recibido a la lista
                vectores_recibidos.append(vector)

            return vectores_recibidos

    except serial.SerialException as e:
        print(f"Error en la comunicación serie: {e}")
        return None


