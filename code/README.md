# Documentación


## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

## Multiplicación de matrices

Este módulo compara experimentalmente dos métodos de multiplicación de matrices cuadradas: Naive y Strassen.

### Programa principal

El programa principal se encarga de recorrer las instancias almacenadas en data/matrix_input, cargar los pares de matrices
correspondientes para ejecutar los algoritmos de multiplicación.

Cada prueba registra: 
-Algoritmo utilizado
-Dimensión de la matriz
-Tipo de matriz
-Dominio de sus elementos
-número de muestra
-tiempo de ejecución
-máximo de memoria residente alcanzado
-correctitud del resultado

El tiempo de ejecución se mide usando std::chrono::steady_clock considerando únicamente la ejecución del algoritmo, mientras
que el consumo de memoria se obtiene mediante getrusage() y el campo ru_maxrss que en Linux corresponde al máximo Resident Set Size alcanzado
en el proceso. Las pruebas se ejecutan en procesos hijos mediante fork() y los resultados de la medición se comunican al proceso principal mediante pipe().

Las mediciones obtenidas se almacenan en:

data/measurements/measurements.txt 

Los campos del archivo se encuentran separados por ";"

La multiplicación clásica utiliza el procedimiento convencional basado en tres ciclos anidados con una complejidad temporal de O(n^3).
Strassen divide las matrices en submatrices reduciendo el número de multiplicaciones de 8 a 7, obteniendo una complejidad teórica de O(n^2.807).

Las referencias utilizadas se encuentran en references.bib


### Scripts

plot_generator.py 

procesa el archivo de mediciones generado por el programa principal para construir gráficos permitiendo analizar el comportamiento experimental de los algoritmos.

Se grafican:

-Tiempo medio de ejecución según la dimensión de la matriz
-Tiempo medio según el tipo de matriz
-Memoria máxima según la dimensión.

Para determinadas visualizaciones se utilizan escalas logarítmicas.
La memoria registrada en KiB se convierte en MiB dividiendo por 1024.

Los gráficos se almacenan en 

data/plots/


## Ordenamiento de arreglo unidimensional

Se compara los siguientes algoritmos de ordenamiento: MergeSort, QuickSort, PatienceSort, std::sort.

### Programa principal

El programa principal recorre los arreglos almacenados en data/array_input para luego ejecutar cada algoritmo
sobre una copia de la instancia original.

Para cada algoritmo se registra:

-Algoritmo
-Tamaño del arreglo (n)
-Tipo de entrada
-Dominio
-Número de muestra
-Tiempo de ejecución 
-Máximo de memoria residente
-correctitud del resultado

El tiempo de ejecución se mide usando std::chrono::steady_clock considerando únicamente la ejecución del algoritmo, mientras
que el consumo de memoria se obtiene mediante getrusage() y el campo ru_maxrss que en Linux corresponde al máximo Resident Set Size alcanzado
en el proceso. Las pruebas se ejecutan en procesos hijos mediante fork() y los resultados de la medición se comunican al proceso principal mediante pipe().

Antes de verificar la validez de la medición se verifica el resultado producido por cada algoritmo.

Las mediciones se almacenan en:

data/measurements/measurements.txt

Se usa ";" como separador

MergeSort

Implementación recursiva basada en divide-and-conquer. El arreglo se divide hasta obtener subarreglos elementalesm para luego
combinar en orden mediante merge.

Su complejidad temporal es O(nlog n)

QuickSort

implementación basada en particionamiento. El arreglo se divide respecto de un pivote y las particiones resultantes se ordenan 
recursivamente.

Su complejidad temporal es O(n log n)
El peor caso es O(n^2)

PatienceSort 

Se distribuye los elementos en pilas siguiendo el procedimiento de Patience Sorting. Posteriormente las pilas son combinadas para obtener
el arreglo ordenado

std::sort

std::sort de la biblioteca estándar de C++ el cual sirve como punto de comparación frente a los algoritmos previamente mencionados.

Las referencias correspondientes a los algoritmos utilizados se encuentran en references.bib

### Scripts

plot_generator.py

Lee las mediciones generadas de measurements.txt y construye gráficos para comparar experimentalmente lso algoritmos 

Se grafican:

-Boxplot del tiempo de ejecución para un tamaño seleccionado
-Boxplot del máximo de memoria residente
-Comparación del tiempo medio según el tipo de entrada
-Evolución del tiempo medio según el tamaño del arreglo

Los gráficos generados se almacenan en 

data/plots/

