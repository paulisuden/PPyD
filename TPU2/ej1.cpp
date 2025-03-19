//   g++ -no-pie -o ej1 ej1.cpp -lpthread
//   ./ej1 5 3


#include <iostream>
#include <pthread.h>
#include <unistd.h> // Para usleep
#include <cstdlib>  // Para rand y srand
#include <ctime>    // Para time

using namespace std;

int Compartida = 0;
pthread_mutex_t mutexCompartida;

void* thread_tipo1(void* arg) { //void* significa que recibe un argumento de cualquier tipo
    int id = *(int*)arg; //y acá debo definir de qué tipo es
    cout << "Thread1 instancia " << id << endl;

    usleep((rand() % 1001) * 1000);
   
    pthread_mutex_lock(&mutexCompartida); //evitar condiciones de carrera cuando múltiples threads acceden y modifican la variable global Compartida.
    Compartida++;
    pthread_mutex_unlock(&mutexCompartida);
   
    pthread_exit(nullptr); //no es necesario pero es una buena práctica para asegurarnos de que el thread termina correctamente.
}

void* thread_tipo2(void* arg) {
    int id = *(int*)arg;
    cout << "Thread2 instancia " << id << endl;
    usleep((rand() % 1001) * 1000); // Tiempo aleatorio entre 0 y 1 segundo
   
    pthread_mutex_lock(&mutexCompartida);
    cout << "Valor de Compartida: " << Compartida << endl;
    pthread_mutex_unlock(&mutexCompartida);
   
    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <N> <M>" << endl;
        return 1;
    }

    int N = atoi(argv[1]); //convierte string a int, (string porque fue lo que se ingresó por consola)
    int M = atoi(argv[2]);

    srand(time(nullptr)); //inicializa la semilla del generador de números aleatorios con el tiempo actual. Permite que cada ejecución tenga tiempos de espera diferentes en usleep.
    pthread_mutex_init(&mutexCompartida, nullptr); //inicializa la mutex compartida

    pthread_t threads1[N], threads2[M]; //define los arrays de tipo pthread_t que Se usa para almacenar el identificador de un thread
    int ids1[N], ids2[M];

    for (int i = 0; i < N; i++) {
        ids1[i] = i;
        pthread_create(&threads1[i], nullptr, thread_tipo1, &ids1[i]);
    }
   
    for (int i = 0; i < M; i++) {
        ids2[i] = i;
        pthread_create(&threads2[i], nullptr, thread_tipo2, &ids2[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads1[i], nullptr);
    }
   
    for (int i = 0; i < M; i++) {
        pthread_join(threads2[i], nullptr);
    }

    pthread_mutex_destroy(&mutexCompartida);
    cout << "Fin del programa" << endl;

    return 0;
}