import pandas as pd
from pathlib import Path
import matplotlib.pyplot as plt


"""
Generación de gráficos para los experimentos de algoritmos de ordenamiento.

Procesa las mediciones obtenidas durante la ejecución de los algoritmos de sorting
y genera distintas visualizaciones para comparar su comportamiento experimental.

Los datos se leen desde:

    data/measurements/measurements.txt

El archivo utiliza ";" y :

    algoritmo : algoritmo de ordenamiento ejecutado.
    n         : tamaño del arreglo de entrada.
    tipo      : ascendente, descendente, aleatoria.
    dominio   : dominio utilizado para los valores.
    tiempo_ms : tiempo de ejecución en milisegundos.
    memoria_kb: máximo de memoria residente en KiB.
    correcto  : indica si el arreglo está ordenado correctamente
    
El script genera:

1. Boxplot del tiempo de ejecución para un tamaño de entrada.
2. Resumen estadístico del tiempo según algoritmo y tipo de entrada.
3. Boxplot del consumo máximo de memoria.
4. Gráfico de barras del tiempo medio según tipo de entrada.
5. Evolución del tiempo medio según n.


NOTA: 
-Se excluyen las instancias de tamaño 10^7 para el grafico tiempo_vs_n porque
no se obtuvieron las mediciones completas para todos los algoritmos.

"""


BASE_DIR = Path(__file__).resolve().parent.parent

archivo = BASE_DIR / "data" / "measurements" / "measurements.txt"

PLOTS_DIR = BASE_DIR / "data"/ "plots"

PLOTS_DIR.mkdir(exist_ok=True)

df = pd.read_csv(archivo, sep = ";")



##################################
###Boxplot tiempo de ejecución####
##################################


n_seleccionado = 100000

df_n = df[df["n"] == n_seleccionado]

algoritmos = df_n["algoritmo"].unique()

datos = []

for algoritmo in algoritmos:
    tiempos = df_n[
        df_n["algoritmo"] == algoritmo
    ]["tiempo_ms"]

    datos.append(tiempos)

plt.figure(figsize=(8,6))


plt.boxplot(datos,tick_labels=algoritmos)
plt.xlabel("Algoritmo")
plt.ylabel("Tiempo de ejecución (ms)")

plt.title(
    f"Distribución del tiempo de ejecución para n = {n_seleccionado}"
)

plt.grid( axis = "y", alpha = 0.3)

plt.tight_layout()
plt.yscale("log")

ruta_salida = (PLOTS_DIR / f"boxplot_tiempo_{n_seleccionado}.png")

plt.savefig(ruta_salida, dpi=300)

plt.show()
print("Gráfico guardado en: ")
print(ruta_salida)



resumen = (
    df[df["n"] == 100000]
    .groupby(["algoritmo", "tipo"])["tiempo_ms"]
    .agg(["mean", "median", "min", "max"])
)





#######################
###BOX PLOT MEMORIA####
#######################


df_n = df[(df["n"] == n_seleccionado) & (df["correcto"] == 1)].copy()

df_n["memoria_mb"] = df_n["memoria_kb"]/1024

algoritmos = df_n["algoritmo"].unique()

datos_memoria = []

for algoritmo in algoritmos:
    memoria = df_n[
        df_n["algoritmo"] == algoritmo]["memoria_mb"]

    datos_memoria.append(memoria)

plt.figure(figsize=(8,6))
plt.boxplot(datos_memoria,tick_labels=algoritmos)

plt.xlabel("Algoritmo")
plt.ylabel("Memoria máxima utilizada (MiB)")

plt.title(f"Distribución del consumo de memoria para n = {n_seleccionado}")

plt.grid(axis="y",alpha=0.3)

plt.tight_layout()

ruta_salida=(PLOTS_DIR / f"boxplot_memoria_{n_seleccionado}.png")

plt.savefig(ruta_salida,dpi=300)

plt.show()


##############################################
###Gráfico de barra: Tiempo medio por tipo####
##############################################


df_n = df[(df["n"] == n_seleccionado) & (df["correcto"] == 1)]

medias = (df_n.groupby(["algoritmo","tipo"])["tiempo_ms"]
          .mean()
          .unstack())



ax = medias.plot(kind="bar",figsize=(9,6))
ax.set_yscale("log")

ax.set_xlabel("Algoritmo")
ax.set_ylabel("Tiempo medio de ejecución (ms)")
ax.set_title(f"Tiempo medio de ejecución según tipo de entrada n = {n_seleccionado}")

ax.legend(title = "Tipo de entrada")

ax.grid(axis = "y", alpha =0.3)

plt.xticks(rotation = 0)
plt.tight_layout()

ruta_salida = (PLOTS_DIR / f"barras_tiempo_tipo_{n_seleccionado}.png")

plt.savefig(ruta_salida,dpi=300)

plt.show()


##############################################
###    Tiempo medio : tamaño de entrada   ####
##############################################


tiempo_por_n = (
    df[ (df["correcto"] == 1) & (df["n"] != 10000000) ]
    .groupby(["algoritmo" , "n"])["tiempo_ms"]
    .mean()
    .reset_index()
)

plt.figure(figsize= (9,6))

for algoritmo in tiempo_por_n["algoritmo"].unique():
    datos_alg = tiempo_por_n[
        tiempo_por_n["algoritmo"] == algoritmo
    ]

    plt.plot(datos_alg["n"],
             datos_alg["tiempo_ms"],
             marker="o",
             label=algoritmo)

plt.xscale("log")
plt.yscale("log")

plt.xlabel("Tamaño de entrada (n)")
plt.ylabel("Tiempo medio de ejecución (ms)")
plt.title("Tiempo de ejecución según tamaño de entrada")

plt.legend()
plt.grid(True, alpha = 0.3)

plt.tight_layout()

ruta_salida = PLOTS_DIR / "tiempo_vs_n.png"
plt.savefig(ruta_salida, dpi=300)

plt.show()





