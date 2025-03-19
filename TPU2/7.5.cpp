//   g++ -o 7.5 7.5.cpp 
//   ./7.5 2

#include <iostream>
#include <unistd.h>  // Para fork y usleep
#include <cstdlib>   // Para rand y srand
#include <ctime>     // Para time   
#include <sys/wait.h> // Para wait

using namespace std;

std::string cartas[] = {"As", "2", "3", "4", "5", "6", "7", "sota", "rey", "caballo"};
float valores[] = {1, 2, 3, 4, 5, 6, 7, 0.5, 0.5, 0.5};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <N> " << endl;
        return 1;
    }

    int N = atoi(argv[1]);
    int pipesCartas[N][2]; // Pipe para enviar cartas
    int pipesResultados[N][2]; // Pipe para recibir puntajes
    pid_t hijos[N];
    bool activos[N];

    std::srand(std::time(0));

    for (int i = 0; i < N; i++) {
        if (pipe(pipesCartas[i]) == -1 || pipe(pipesResultados[i]) == -1) {
            cerr << "Error al crear los pipes " << i << endl;
            return 1;
        }
        activos[i] = true;
    }

    // CREO PROCESOS HIJOS
    for (int i = 0; i < N; i++) {
        hijos[i] = fork();

        if (hijos[i] < 0) {
            cerr << "Error al crear el proceso hijo " << i << endl;
            return 1;
        }

        // Código del hijo
        if (hijos[i] == 0) {
            close(pipesCartas[i][1]); // Cierra escritura (solo lee cartas)
            close(pipesResultados[i][0]); // Cierra lectura (solo escribe el resultado)

            float puntaje = 0;

            while (true) {
                char buffer[20] = {0};
                ssize_t bytesLeidos = read(pipesCartas[i][0], buffer, sizeof(buffer));

                if (bytesLeidos <= 0) {
                    break;
                }

                std::string carta(buffer);
                float valor = 0;

                // Buscar el valor de la carta
                for (int j = 0; j < 10; j++) {
                    if (cartas[j] == carta) {
                        valor = valores[j];
                        break;
                    }
                }

                // Sumar el puntaje
                puntaje += valor;

                cout << "Jugador " << i + 1 << " recibió: " << carta
                     << " (Total: " << puntaje << " puntos)" << endl;

                // Evaluar si se planta o abandona
                if (puntaje == 7 || puntaje == 7.5) {
                    cout << "Jugador " << i + 1 << " se planta con " << puntaje << " puntos.\n";
                    write(pipesResultados[i][1], &puntaje, sizeof(puntaje));
                    break;
                } else if (puntaje > 7.5) {
                    cout << "Jugador " << i + 1 << " se pasó con " << puntaje << " puntos y queda eliminado.\n";
                    write(pipesResultados[i][1], &puntaje, sizeof(puntaje));
                    break;
                }
            }

            // Cerrar pipes antes de salir
            close(pipesCartas[i][0]); 
            close(pipesResultados[i][1]); 

            exit(0); 
        }
    }

    // Código del padre
    bool juegoActivo = true;
    while (juegoActivo) {
        juegoActivo = false;

        for (int i = 0; i < N; i++) {
            if (activos[i]) {
                int status;
                pid_t result = waitpid(hijos[i], &status, WNOHANG);
                if (result > 0) {
                    activos[i] = false;
                    close(pipesCartas[i][1]);
                    continue;
                }

                // Elegir carta
                int indice = std::rand() % 10;
                std::string cartaSeleccionada = cartas[indice];

                write(pipesCartas[i][1], cartaSeleccionada.c_str(), cartaSeleccionada.size() + 1);
                
                juegoActivo = true; 
            }
        }
        
        sleep(1); 
    }

    // Cerrar pipes de escritura en el padre
    for (int i = 0; i < N; i++) {
        close(pipesCartas[i][1]);
    }

    // Leer puntajes finales y determinar el ganador
    float mejorPuntaje = 0;
    int ganador = 0;

    for (int i = 0; i < N; i++) {
        float puntajeFinal;
        int status;

        waitpid(hijos[i], &status, 0); // Espera a que el hijo termine

        read(pipesResultados[i][0], &puntajeFinal, sizeof(puntajeFinal));
        close(pipesResultados[i][0]); 

        cout << "Jugador " << i + 1 << " terminó con " << puntajeFinal << " puntos.\n";

        if (puntajeFinal > mejorPuntaje && puntajeFinal <= 7.5) {  
            mejorPuntaje = puntajeFinal;
            ganador = i + 1;
        }
    }

    if (ganador == 0) {
        cout << "Ningún jugador ganó.\n";
    } else {
        cout << "JUGADOR " << ganador << " GANÓ LA PARTIDA CON " << mejorPuntaje << " PUNTOS.\n";
    }

    return 0;
}
