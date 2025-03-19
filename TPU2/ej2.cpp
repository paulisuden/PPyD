//   g++ -o ej2 ej2.cpp 
//   ./ej2 5 3


#include <iostream>
#include <unistd.h>  // Para fork y usleep
#include <cstdlib>   // Para rand y srand
#include <ctime>     // Para time
#include <sys/mman.h> // Para memoria compartida
#include <sys/stat.h> // Para constantes de modo
#include <fcntl.h>    
#include <sys/wait.h> // Para wait

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <N> <M>" << endl;
        return 1;
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    
    srand(time(nullptr)); // Inicializa la semilla del generador de números aleatorios

    // Crear memoria compartida para la variable Compartida porque cada proceso tiene su propia memoria
    
    int shm_fd = shm_open("/compartida", O_CREAT | O_RDWR, 0666); //permisos para crear, leer y escribir
    
    ftruncate(shm_fd, sizeof(int));  //tamaño de la memoria compartida
    
    int* Compartida = (int*) mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                    //MAP_SHARED → Los cambios se reflejan en otros procesos.
    
    *Compartida = 0; // Inicializa la variable compartida

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo tipo 1
            cout << "Proceso1 instancia " << i << endl;
            usleep((rand() % 1001) * 1000);
            (*Compartida)++;
            exit(0);
        }
    }

    for (int i = 0; i < M; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo tipo 2
            cout << "Proceso2 instancia " << i << endl;
            usleep((rand() % 1001) * 1000);
            cout << "Valor de Compartida: " << *Compartida << endl;
            exit(0);
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < N + M; i++) {
        wait(nullptr);
    }

    // Liberar la memoria compartida
    munmap(Compartida, sizeof(int)); //Desvincula la memoria compartida del proceso.
    shm_unlink("/compartida"); //elimina la memoria compartida del sistema, evitando fugas de memoria.

    cout << "Fin del programa" << endl;
    return 0;
}


/*
Resumen

Crear la memoria compartida: shm_open

Definir su tamaño: ftruncate

Asignarla al proceso: mmap

Leer y escribir en ella

Liberarla cuando ya no se necesite: munmap

Eliminarla del sistema al final: shm_unlink
*/


