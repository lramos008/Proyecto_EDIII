# -*- coding: utf-8 -*-
"""
Created on Fri Jan 17 16:02:50 2025

@author: dramos
"""

from template_generator import generate_template
import numpy as np
import argparse
import os



# #Genero el template
# template_folder = "conjunto_template"
# template = generate_template(template_folder)
# template.tofile(filename)

def main():
    # Configurar los argumentos de línea de comandos
    parser = argparse.ArgumentParser(description="Generar y guardar un template en un archivo binario.")
    parser.add_argument(
        "filename", 
        type=str, 
        help="Nombre del archivo donde se guardará el template (incluyendo la extensión .bin)."
    )
    parser.add_argument(
        "--folder", 
        type=str, 
        default="conjunto_template", 
        help="Carpeta donde se generará el template (opcional, por defecto 'conjunto_template')."
    )

    # Parsear los argumentos
    args = parser.parse_args()

    # Generar el template
    template = generate_template(args.folder)

    # Asegurarse de que la carpeta exista
    if not os.path.exists(args.folder):
        os.makedirs(args.folder)

    # Crear la ruta completa del archivo
    file_path = os.path.join(args.folder, args.filename)

    # Guardar el template en el archivo especificado
    template.tofile(file_path)
    print(f"Template guardado en {os.path.abspath(file_path)}")

if __name__ == "__main__":
    main()
