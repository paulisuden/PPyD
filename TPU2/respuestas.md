El comportamiento de ambos ejercicios es similar en términos de funcionalidad, pero hay diferencias clave debido a la forma en que los hilos y los procesos manejan la memoria y la concurrencia.  

---

## **Diferencias clave en comportamiento**
| Aspecto | Hilos (`pthread`) | Procesos (`fork`) |
|---------|------------------|------------------|
| **Compartición de memoria** | Todos los hilos comparten la misma memoria dentro del proceso. | Cada proceso tiene su propio espacio de memoria, por lo que se necesita **memoria compartida** para comunicarse. |
| **Velocidad de creación** | Crear un hilo es más rápido porque no necesita copiar todo el espacio de memoria. | Crear un proceso con `fork()` es más costoso porque duplica el espacio de memoria del padre. |
| **Comunicación** | Se puede compartir variables globales sin problemas, pero se deben usar **mutexes** para evitar condiciones de carrera. | Se necesita **memoria compartida** (`shm_open` y `mmap`) para comunicarse entre procesos. |
| **Independencia** | Si un hilo falla, puede afectar a todo el programa. | Si un proceso falla, los demás pueden seguir ejecutándose. |
| **Planificación del sistema** | Los hilos dentro de un mismo proceso se ejecutan en paralelo, pero dependen del programador para sincronizarse. | Cada proceso es independiente y el sistema operativo maneja la planificación. |
| **Uso de CPU y memoria** | Menos consumo de memoria, ya que todos los hilos usan la misma. | Mayor consumo de memoria porque cada proceso tiene su propia copia de variables. |
| **Uso en la práctica** | Ideal para tareas que requieren compartir información rápidamente (ejemplo: servidores web, cálculos paralelos). | Ideal para programas donde cada tarea es independiente (ejemplo: ejecución de múltiples programas en paralelo). |

---

## **Diferencias observadas en la ejecución del ejercicio**
1. **Ejercicio con hilos (`pthread`)**
   - Los hilos de tipo 1 incrementan la variable `Compartida` sin necesidad de memoria compartida explícita.
   - Los hilos de tipo 2 leen la variable `Compartida` fácilmente.
   - Es necesario usar un **mutex** (`pthread_mutex_t`) para evitar condiciones de carrera.
   - Más eficiente en términos de rendimiento porque no hay sobrecarga de creación de procesos.

2. **Ejercicio con procesos (`fork`)**
   - Cada proceso tiene su propia copia de la memoria, por lo que se debe usar **memoria compartida** (`shm_open`, `mmap`).
   - Los procesos tipo 1 incrementan la variable `Compartida` en la memoria compartida.
   - Los procesos tipo 2 leen la variable `Compartida`, pero necesitan acceso a la misma región de memoria compartida.
   - La sincronización es más costosa, ya que se requiere `wait()` para asegurarse de que los procesos terminen.

---

## **¿Cuál es mejor?**
- **Si necesitas compartir datos fácilmente y minimizar la sobrecarga** → **Usa hilos (`pthread`)**.
- **Si necesitas más independencia entre tareas y tolerancia a fallos** → **Usa procesos (`fork`)**.

---

### **Ejemplo práctico**
Piensa en un **navegador web**:
- **Hilos**: Cada pestaña dentro del navegador usa hilos porque comparten la misma memoria y pueden acceder rápidamente a la caché.
- **Procesos**: Algunos navegadores (como Chrome) usan un **proceso por pestaña** para evitar que un error en una pestaña afecte a las demás.

En este ejercicio, el enfoque con **hilos es más eficiente**, pero si buscas **aislamiento** entre tareas, la versión con procesos es mejor.  

¿Te quedó claro o quieres que profundice en alguna parte? 🚀