import pandas as pd
from pathlib import Path
import matplotlib.pyplot as plt



""""
Generación de gráficos para los experimentos de multiplicación de matrices.

Este script procesa las mediciones obtenidas durante la ejecución de los algoritmos
de multiplicación de matrices y genera gráficos comparativos de tiempo de ejecución
y consumo de memoria.

Los datos se leen desde:
    data/measurements/measurements.txt

measurements.txt utiliza ";" como separador. Para el análisis se consideran 
únicamenmte las ejecuciones marcadas como correctas.

El script genera los siguientes gráficos:

1. Tiempo medio de ejecución según la dimensión de n de las matrices.
2. Tiempo medio de ejecución según el tipo de matriz para una dimension seleccionada
3. Memoria máxima media según la dimensión n.

Los resultados gráficos se almacenan en:
    data/plots/


NOTA: se utilizan escalas logarítmicas debido al crecimiento de los tiempos y
a que las dimensiones de las matrices aumentan en potencias de 2.

"""



BASE_DIR = Path(__file__).resolve().parent.parent

archivo = BASE_DIR / "data" / "measurements" / "measurements.txt"

PLOTS_DIR = BASE_DIR / "data"/ "plots"

PLOTS_DIR.mkdir(exist_ok=True)

df = pd.read_csv(archivo, sep = ";")


df = df[df["correcto"] == 1]

tiempo_por_n = (
    df.groupby(["algoritmo", "n"])["tiempo_ms"].mean().reset_index()
)

plt.figure(figsize=(9,6))

for algoritmo in tiempo_por_n["algoritmo"].unique():
    datos_algoritmo = tiempo_por_n[tiempo_por_n["algoritmo"] == algoritmo].sort_values("n")
    plt.plot(datos_algoritmo["n"], datos_algoritmo["tiempo_ms"],marker="o",label = algoritmo)

plt.yscale("log")
plt.xscale("log", base =2)

plt.xlabel("Dimensión de la matriz (n)")
plt.ylabel("Tiempo medio de ejecución (ms)")

plt.title("Tiempo de ejecución según dimensión de la matriz")

plt.legend()

plt.grid(True,alpha=0.3)

plt.tight_layout()

ruta_salida = PLOTS_DIR / "tiempo_vs_n.png"

plt.savefig(ruta_salida,dpi=300)

plt.show()

####################################
##### Gráfico de Barras ############
####################################

n_seleccionado = 256

df_n = df[df["n"] == n_seleccionado]

medias = (df_n.groupby(["algoritmo","tipo"]) ["tiempo_ms"].mean().unstack())


orden_tipo = ["dispersa", "diagonal", "densa"]
medias = medias.reindex(columns=orden_tipo)

ax = medias.plot(kind = "bar" , figsize=(9,6))

ax.set_xlabel("Algoritmo")
ax.set_ylabel("Tiempo medio de ejecución (ms)")
ax.set_title(f"Tiempo medio según tipo de matriz para n = {n_seleccionado}")

ax.legend(title="Tipo de matriz")
ax.grid(axis="y", alpha = 0.3)

plt.xticks(rotation=0)
plt.tight_layout()
plt.yscale("log")

ruta_salida = PLOTS_DIR / f"barras_tiempo_tipo_matriz_{n_seleccionado}.png"

plt.savefig(ruta_salida, dpi=300)
plt.show()


####################################
##### Memoria media vs n ###########
####################################

df_correctos = df[df["correcto"] == 1].copy()

df_correctos["memoria_mib"] = (
    df_correctos["memoria_kb"] / 1024
)


memoria_por_n = df_correctos.groupby(["algoritmo","n"])["memoria_mib"].mean().reset_index()

plt.figure(figsize=(9,6))
for algoritmo in memoria_por_n["algoritmo"].unique():
    datos_algoritmo = memoria_por_n[memoria_por_n["algoritmo"] == algoritmo].sort_values("n")

    plt.plot(datos_algoritmo["n"],datos_algoritmo["memoria_mib"], marker="o", label = algoritmo)


plt.xscale("log",base = 2)

plt.xlabel("Dimensión de la matriz(n)")
plt.ylabel("Memoria máxima media (Mib)")

plt.title("Consumo de memoria según dimensión de la matriz")

plt.legend()

plt.grid(True,alpha=0.3)

plt.tight_layout()

ruta_salida = PLOTS_DIR / "memoria_vs_n.png"

plt.savefig(ruta_salida , dpi = 300)

plt.show()