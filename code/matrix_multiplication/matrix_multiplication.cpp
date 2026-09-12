#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iostream>


//mediciones tiempo
#include <chrono>



//medicion memoria
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>



#include "algorithms/naive.h"
#include "algorithms/strassen.h"

using namespace std;
using Matrix = std::vector<std::vector<int>>;


struct Medicion{
    double tiempo_ms;
    long memoria_kb;
    bool correcto;
};



// funcion auxiliar para construir el nombre del archivo que contiene la matriz
std::string construirNombreArchivo(int n, const std::string& tipo, const std::string& dominio,
    const std::string& muestra, int num){

        return std::to_string(n)
            + "_"
            + tipo
            + "_"
            + dominio
            + "_"
            + muestra
            + "_"
            + to_string(num)
            + ".txt";


}


// funcion auxiliar para guardar una matriz en una ruta filename
void guardarMatriz( const Matrix& matriz, const std::string& filename){

    std::ofstream file(filename);

    if(!file.is_open()){
        cerr << "error al guardar matriz" << endl;
        return; 
    }

    for(const auto& fila : matriz){
        for(size_t j = 0 ; j < fila.size() ; j++){
            file << fila[j];

            if (j + 1 < fila.size()){
                file << " ";
            }
        }

        file << "\n";
    }

}









/*
 * Ejecuta un algoritmo y obtiene las métricas experimentales de
 * tiempo, memoria y correctitud.
 *
 * Primero empieza al crearse un proceso hijo fork() para ejecutar
 * cada prueba de manera independiente.
 *
 * El tiempo de ejecución se mide utilizando std::chrono::steady_clock,
 * marcando como inicio y final inmediatamente antes y después de ejecutar
 * el algoritmo.
 *
 * El uso de memoria se obtiene mediante getrusage()
 * utilizando el campo ru_maxrss el cual corresponde 
 * al máximo Resident Set Size alcanzado en el proceso 
 * en KiB.
 *
 * Posteriormente, se comprueba la correctitud del resultado y para finalizar
 * las mediciones son enviadas desde el proceso hijo al proceso padre mediante un pipe.
 *
 * NOTA: La medición de tiempo considera únicamente la ejecución del algoritmo excluyendo
 * el tiempo de lectura y escritura de archivos

*/

Medicion ejecutarTest(
    const Matrix& A,
    const Matrix& B,
    const Matrix& esperado,
    const std::string& algoritmo,
    int n,
    const std::string& t,
    const std::string& d,
    string m
){

    int fd[2];

    if (pipe(fd) == -1){
        std::cerr << "error al crear pipe\n";
        return {-1.0,-1,false};
    }

    pid_t pid = fork();

    if(pid < 0 ){
        std::cerr << "error al crear proceso hijo\n";
        return {-1.0,-1,false};
    }

    if (pid == 0){
        close(fd[0]);

        Matrix A_copia = A;
        Matrix B_copia = B;
        Matrix resultado;

        auto inicio = std::chrono::steady_clock::now();

        if(algoritmo == "naive"){
            resultado= naive(A_copia,B_copia);
        }else if (algoritmo == "strassen"){
            resultado = strassenMultiply(A_copia,B_copia);
        }
        else {
            close(fd[1]);
            exit(EXIT_FAILURE);
        }
        auto fin = std::chrono::steady_clock::now();
        double tiempo = std::chrono::duration<double, std::milli>(fin-inicio).count();
        bool correcto = (resultado == esperado);

        std::string archivoSalida= "data/matrix_output/" + std::to_string(n) + "_" + t + "_" + d + "_" + m + "_" +
            algoritmo + ".txt";

        guardarMatriz(resultado, archivoSalida);

        write(fd[1], &tiempo, sizeof(tiempo));
        close(fd[1]);

        exit(correcto ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    //PADRE
    close(fd[1]);
    struct rusage uso;
    int status;

    wait4(pid,&status,0,&uso);

    double tiempo = -1.0;

    read(fd[0], &tiempo, sizeof(tiempo));

    close(fd[0]);

    long memoriaKB = uso.ru_maxrss;

    bool correcto = WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS;

    cout << algoritmo << " | m = " << n
                      << " | tiempo = " << tiempo << " ms"
                      << " | memoria = " << memoriaKB << " kb"
                      << " | correcto = " << correcto << endl;

    return {tiempo,memoriaKB, correcto};

}


// lee la matriz ubicada en ruta y lo guarda en la variable matriz
bool leerMatriz(const std::string& ruta, std::vector<std::vector<int>>& matriz){

    std::ifstream archivo(ruta);

    if(!archivo.is_open()){
        std::cerr << "No se pudo abrir el archivo:"
                  << ruta << std::endl;
        return false;
    }

    matriz.clear();
    std::string linea;
    while(std::getline(archivo,linea)){

        if(linea.empty()){
            continue;
        }
        std::stringstream ss(linea);
        std::vector<int> fila;
        int valor;
        while(ss >> valor){
            fila.push_back(valor);
        }
        if (!fila.empty()){
            matriz.push_back(fila);
        }
    }
    archivo.close();
    if( matriz.empty()){
        std::cerr << "matriz vacia: " << ruta << std::endl;
        return false;
    }
    return true;
}

/*
 * Programa principal de evaluación de algoritmos de multiplicación de matrices.
 *
 * Recorre todas las combinaciones de dimensión, tipo de matriz, dominio y muestra
 * definidas para los experimentos. Para cada caso:
 *
 * -construye las rutas de las matrices de entrada;
 * -carga las matrices A y B;
 * -calcula el resultado de referencia mediante algoritmo Naive;
 * -mide el tiempo de ejecución, memoria máxima utilizada y correctitud para Naive y Strassen;
 * -guarda las mediciones en data/measurements/measurements.txt
 *
*/

int main()
{
    vector<int> dimensiones = {
        16,64,256,1024
    };

    vector<string> tipos = { 
        "dispersa",
        "diagonal",
        "densa"
    };

    vector<string> dominios = {
        "D0",
        "D10"
    };

    vector<string> muestras = {
        "a",
        "b",
        "c"
    };

    std::ofstream measurements("data/measurements/measurements.txt");

    if(!measurements.is_open()){
        cout<< "error al abrir measurements" << endl;
    }

    measurements << "algoritmo;n;tipo;dominio;muestra;tiempo_ms;memoria_kb;correcto\n";

    for(int n : dimensiones){
        for(const string& t : tipos){
            for(const string& d : dominios){
                for(const string& m : muestras){

                    string base = to_string(n) + "_" + t + "_" + d + "_" + m;

                    string rutaA = "data/matrix_input/" + base + "_1.txt";
                    string rutaB = "data/matrix_input/" + base + "_2.txt";

                    cout<< "procesando " << base << endl;

                    vector<vector<int>> A;
                    vector<vector<int>> B;
                    leerMatriz(rutaA,A);
                    leerMatriz(rutaB,B);

                    //sol referencia
                    vector<vector<int>> resultadoNaive= naive(A,B);
                    
                    Medicion med_naive = ejecutarTest(A,B,resultadoNaive,"naive",n,t,d,m);
                    Medicion med_strassen = ejecutarTest(A,B,resultadoNaive,"strassen",n,t,d,m);

                    measurements 
                    << "naive" << ";"
                    << n << ";"
                    << t << ";"
                    << d << ";"
                    << m << ";"
                    << med_naive.tiempo_ms << ";"
                    << med_naive.memoria_kb << ";"
                    << med_naive.correcto 
                    << endl;

                    measurements 
                    << "strassen" << ";"
                    << n << ";"
                    << t << ";"
                    << d << ";"
                    << m << ";"
                    << med_strassen.tiempo_ms << ";"
                    << med_strassen.memoria_kb << ";"
                    << med_strassen.correcto
                    << endl;



                }
            }
        }
    }


    cout<< "finished! " << endl;
    return 0;
}