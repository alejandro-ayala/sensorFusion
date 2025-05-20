import os
import re
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # aunque no se use explícito, necesario para 3d


def extraer_bloques_y_guardar(input_file, output_prefix="LidarSample_"):
    start_marker = "[DEBUG] 3D image captured in"
    end_marker = "[INFO] SystemTasks::image3dCapturerTask executed in"
    block_lines = []
    capturing = False
    count = 0

    with open(input_file, "r", encoding="utf-8") as f:
        for line in f:
            if start_marker in line:
                capturing = True
                block_lines = []
                continue
            elif end_marker in line and capturing:
                capturing = False
                count += 1
                output_file = f"{output_prefix}{count}.txt"
                with open(output_file, "w", encoding="utf-8") as outf:
                    outf.writelines(block_lines)
                print(f"Guardado bloque {count} en: {output_file}")
                continue

            if capturing:
                # Procesar líneas que empiezan con [INFO]
                if line.startswith("[INFO]"):
                    data_str = line[len("[INFO] "):].strip()
                    data_str = data_str.replace(",", " ")
                    valores = data_str.split()
                    valores_float_str = [f"{float(v):.1f}" for v in valores]
                    linea_procesada = " ".join(valores_float_str) + "\n"
                    block_lines.append(linea_procesada)
                else:
                    block_lines.append(line)


def margen(limites, porcentaje=0.1):
    rango = limites[1] - limites[0]
    return (limites[0] - rango * porcentaje, limites[1] + rango * porcentaje)


def mostrar_nubes_puntos(file_pattern="LidarSample_"):
    # Busca todos los archivos LidarSample_*.txt ordenados por número
    files = [f for f in os.listdir() if f.startswith(file_pattern) and f.endswith(".txt")]
    
    # Ordenar por número al final del nombre (ej: LidarSample_2.txt)
    files.sort(key=lambda x: int(re.findall(r'\d+', x)[0]))

    for archivo_lidar in files:
        print(f"Procesando archivo: {archivo_lidar}")
        try:
            datos_lidar = np.loadtxt(archivo_lidar, usecols=(0, 1, 2))
        except Exception as e:
            print(f"Error cargando {archivo_lidar}: {e}")
            continue

        angulosH = np.radians(datos_lidar[:, 0])
        angulosV = np.radians(datos_lidar[:, 1])
        distancias = datos_lidar[:, 2]

        x = distancias * np.sin(angulosV) * np.cos(angulosH)
        y = distancias * np.sin(angulosV) * np.sin(angulosH)
        z = distancias * np.cos(angulosV)

        xlim = margen((np.min(x), np.max(x)))
        ylim = margen((np.min(y), np.max(y)))
        zlim = margen((np.min(z), np.max(z)))

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        p = ax.scatter(x, y, z, c=z, cmap='viridis', s=3)
        fig.colorbar(p, label='Altura (Z)')

        ax.set_title(f"Nube de puntos: {archivo_lidar}")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")

        ax.set_xlim(xlim)
        ax.set_ylim(ylim)
        ax.set_zlim(zlim)

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    archivo_log = "logLidarData.log"  # Cambia al nombre real de tu fichero log

    print("Extrayendo bloques del log...")
    extraer_bloques_y_guardar(archivo_log)

    print("\nMostrando nubes de puntos de los archivos extraídos...")
    mostrar_nubes_puntos()
