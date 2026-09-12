#include <iostream>
#include <vector>
#include <fstream>
#include <string> 
#include <filesystem>
#include <algorithm>

//mediciones tiempo
#include <chrono>



//medicion memoria
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>


struct Medicion{
    double tiempo_ms;
    long memoria_kb;
    bool correcto;
};




//Algoritmos

auto inicio = std::chrono::high_resolution_clock::now();

void mergeSort(std::vector<int>& arr, int left, int right);

void quickSort(std::vector<int>& vec, int low, int high);

std::vector<int> patienceSorting(std::vector<int>& arr);

std::vector<int> sortArray(std::vector<int>& arr);


std::string construirNombreArchivo(int n, const std::string& tipo, const std::string& dominio,
    const std::string& muestra){

        return std::to_string(n)
            + "_"
            + tipo
            + "_"
            + dominio
            + "_"
            + muestra
            + ".txt";

}





//leer arreglo

std::vector<int> leerArreglo(const std::string& ruta){
    std::ifstream archivo(ruta);

    if(!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir ";
        std::cerr << ruta << "/n";
        
        return {};
    }


    std::vector<int> respuesta;
    int num_elem;

    while (archivo >> num_elem){
        respuesta.push_back(num_elem);
    }

    archivo.close();
    return respuesta;

}



bool guardarArreglo(const::std::string& ruta, const::std::vector<int>& arreglo){
    std::ofstream archivo(ruta);

    if(!archivo.is_open()){
        std::cerr << "Error guardarArreglo"<< std::endl;
        return false;
    }

    for(std::size_t i = 0 ; i < arreglo.size() ; i++){
        archivo << arreglo[i];

        if (i+1 < arreglo.size()){
            archivo << " ";
        }
    }

    archivo << "\n";

    return true;
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

Medicion ejecutarTest(const std::string& algoritmo, const std::vector<int>& original, const std::string& rutaSalida ){


    std::vector<int> datos = original;

    int fd[2];

    if(pipe(fd) == -1) {
        std::cerr<< "error de pipe\n";
        return{-1.0,-1,false};
    }

    pid_t pid = fork();

    if (pid < 0 ){
        std::cerr <<"error al crear hijo\n";

        close(fd[0]);
        close(fd[1]);
        return{-1.0,-1,false};
    }


    if (pid == 0 ){
        close(fd[0]);

        auto inicio = std::chrono::steady_clock::now();

        if(algoritmo == "mergesort"){
            if(!datos.empty()){
                mergeSort(datos,0, static_cast<int>(datos.size()) - 1);
            }
        } else if (algoritmo == "patiencesort"){
            if(!datos.empty()){
                datos = patienceSorting(datos);
            }
        } else if (algoritmo == "quicksort"){
            if(!datos.empty()){
                quickSort(datos, 0 , static_cast<int>(datos.size() - 1));
            }
        } else if (algoritmo == "sort"){
            if(!datos.empty()){
                datos = sortArray(datos);
            }
        } else {
            close(fd[1]);
            _exit(2);
        }


        auto fin = std::chrono::steady_clock::now();

        double tiempo_ms = std::chrono::duration<double, std::milli>(fin - inicio).count();

        bool correcto = std::is_sorted(datos.begin(),datos.end());

        struct rusage uso;

        getrusage(RUSAGE_SELF, &uso);

        long memoria_kb = uso.ru_maxrss;

        Medicion medicion {tiempo_ms,memoria_kb,correcto};

        write (fd[1], &medicion, sizeof(medicion));

        close(fd[1]);

        bool guardado = guardarArreglo(rutaSalida, datos);

        if(!correcto || !guardado){
            _exit(3);
        }


        _exit(0);

    }

    close(fd[1]);

    Medicion medicion{-1.0,-1, false};

    read(fd[0], &medicion, sizeof(medicion));

    close(fd[0]);

    int status;

    waitpid(pid, &status,0);

    if(!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        medicion.correcto = false;
    }

    return medicion;

}



/*
 * Programa principal de evaluación de algoritmos de ordenanmiento.
 *
 * Recorre todas las combinaciones de tamaño, tipo de arreglo, dominio y muestra
 * definidas para los experimentos, para cada archivo de entrada:
 * 
 *
 * - carga el arreglo original:
 * - ejecute Merge sort, Patience Sort, QuickSort y std::sort;
 * - registra tiempo de ejecución, memoria máxima utilizada y correctitud;
 * - guarda el arreglo ordenado generado por cada algoritmo;
 * - almacena las mediciones en data/measurements/measurements.txt
 *
 * Para n= 10^7 se omiten Patience Sort y algunos casos de QuickSort debido a
 * sus altos tiempos de ejecución en entradas desfavorables

*/


int main() {

    std::vector<int> sizes{10, 1000, 100000, 10000000 
    };


    std::vector<std::string> tipos{ "ascendente", "descendente", "aleatorio"

    };

    std::vector<std::string> dominios{ "D1" , "D7"

    };

    std::vector<std::string> muestras{"a","b","c"};


    std::filesystem::create_directories("data/measurements");

    std::filesystem::create_directories("data/array_output/mergesort");
    std::filesystem::create_directories("data/array_output/patiencesort");
    std::filesystem::create_directories("data/array_output/quicksort");
    std::filesystem::create_directories("data/array_output/sort");

    std::ofstream measurements("data/measurements/measurements.txt");

    if(!measurements.is_open()){
        std::cerr
            << "No se pudo abrir measurements"<< std::endl;
        return 1;
    }
    

    //cabecera

    measurements 
        << "algoritmo;"
        << "n;"
        << "tipo;"
        << "dominio;"
        << "muestra;"
        << "tiempo_ms;"
        <<  "memoria_kb;"
        <<  "correcto\n";


    for(int n : sizes){
        for(const std::string& tipo : tipos){
            for(const std::string& dom: dominios){
                for(const std::string& muestr : muestras){

                    std::string file_input_name = construirNombreArchivo(n,tipo,dom,muestr);
                    std::string rute_input = "data/array_input/" + file_input_name;

                    std::vector<int> original = leerArreglo(rute_input);

                    std::cout << "Leyendo " << rute_input << std::endl;

                    if(original.empty()){
                        std::cerr << "error no se pudo leer" << rute_input << std::endl;
                        continue;
                    }

                    std::string nombreSalida = std::to_string(n) + "-" + tipo + "-" + dom + "-" + muestr + "_out.txt";




                    ////////////////////////
                    // mergesort ///////////
                    ////////////////////////


                    Medicion merge = ejecutarTest("mergesort",original,"data/array_output/mergesort/" + nombreSalida);

                    measurements 
                        << "mergesort;"
                        << n << ";"
                        << tipo << ";"
                        << dom << ";"
                        << muestr << ";"
                        << merge.tiempo_ms << ";"
                        << merge.memoria_kb << ";"
                        << merge.correcto << "\n";


                    std::cout<< "MergeSort ha sido completado"<< std::endl;

                    ////////////////////////
                    // patiencesort ////////
                    ////////////////////////


                    if(n != 10000000){

                        Medicion patience  = ejecutarTest("patiencesort",original,"data/array_output/patiencesort/" + nombreSalida);
    
                    
                        measurements 
                            << "patiencesort;"
                            << n << ";"
                            << tipo << ";"
                            << dom << ";"
                            << muestr << ";"
                            << patience.tiempo_ms << ";"
                            << patience.memoria_kb << ";"
                            << patience.correcto << "\n";
    
    
    
                        std::cout<< "PatienceSort ha sido completado"<< std::endl;


                    }

                    ////////////////////
                    // QuickSort////////
                    ////////////////////


                    if(n != 10000000 || tipo == "aleatorio" ){
                        Medicion quick = ejecutarTest("quicksort",original,"data/array_output/quicksort/" + nombreSalida);
    
                        measurements 
                            << "quicksort;"
                            << n << ";"
                            << tipo << ";"
                            << dom << ";"
                            << muestr << ";"
                            << quick.tiempo_ms << ";"
                            << quick.memoria_kb << ";"
                            << quick.correcto << "\n";
    
    
                        std::cout<< "QuickSort ha sido completado"<< std::endl;


                    } 


                    ////////////////////
                    // STD::SORT////////
                    ////////////////////
                    


                    Medicion standard = ejecutarTest("sort",original,"data/array_output/sort/" + nombreSalida);

                    measurements 
                        << "sort;"
                        << n << ";"
                        << tipo << ";"
                        << dom << ";"
                        << muestr << ";"
                        << standard.tiempo_ms << ";"
                        << standard.memoria_kb << ";"
                        << standard.correcto << "\n";

                    
                    std::cout<< "Sort ha sido completado"<< std::endl;

                    measurements.flush();
                }
            }
        }
    }



    std::cout << "finished! " << std::endl;
    return 0;

}