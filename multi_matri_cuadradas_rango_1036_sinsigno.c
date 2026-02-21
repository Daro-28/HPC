#include <stdio.h>
#include <stdlib.h>
#include <time.h>    // clock(), time(), struct timespec
#include <errno.h>   // errno (para strtoul)
#include <stdint.h>  // uint32_t, uint64_t
#include <inttypes.h> // PRIu32 (opcional para prints de uint32_t)

/**
 * Multiplicacion de Matrices Cuadradas
 * Medicion de Wall Clock Time vs CPU Time
 * Para ejecutar en terminal: 
gcc -std=c11 -O0 -Wall -Wextra multi_matri_cuadradas_rango_1036_sinsigno.c -o multi_matri_cuadradas_rango_1036_sinsigno

luego

./multi_matri_cuadradas_rango_1036_sinsigno 1000
 */
int main(int argc, char *argv[]) {
    /* 
    argc: Número de argumentos de línea de comandos 
    argv: Array de strings con los argumentos pasados al programa 
    n: Tamaño de la matriz (N x N) (positivo)
    struct timespec: Estructura que almacena segundos y nanosegundos (para tiempo real)
    clock_t: Tipo para almacenar tiempo de CPU 
    uint32_t: Entero sin signo de 32 bits (0 .. 4,294,967,295)
    */

    size_t n;

    // Variables para Wall Clock (tiempo real)
    struct timespec wall_inicio, wall_fin;
    // Variables para CPU Time (tiempo de procesador)
    clock_t cpu_inicio, cpu_fin;

    // 1. Entrada de datos (SOLO por línea de comandos)
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\nEjemplo: %s 100\n", argv[0], argv[0]);
        return 1;
    }

    // Convertir argv[1] a entero sin signo de forma segura (mejor que atoi)
    errno = 0;
    char *endptr = NULL;
    unsigned long tmp = strtoul(argv[1], &endptr, 10);

    // Validaciones: conversión correcta, sin basura, sin overflow
    if (errno != 0 || endptr == argv[1] || *endptr != '\0') {
        fprintf(stderr, "Error: N debe ser un entero válido.\n");
        return 1;
    }

    if (tmp == 0 || tmp > 100000UL) { // límite arbitrario para evitar valores absurdos
        fprintf(stderr, "Error: N debe ser > 0 y razonable.\n");
        return 1;
    }

    n = (size_t)tmp;

    // 2. Reserva de memoria (matrices de ENTEROS SIN SIGNO de 32 bits)
    uint32_t **A = (uint32_t **)malloc(n * sizeof(uint32_t *));
    uint32_t **B = (uint32_t **)malloc(n * sizeof(uint32_t *));
    uint32_t **C = (uint32_t **)malloc(n * sizeof(uint32_t *));
    if (!A || !B || !C) {
        fprintf(stderr, "Error: no se pudo reservar memoria para punteros.\n");
        free(A); free(B); free(C);
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        A[i] = (uint32_t *)malloc(n * sizeof(uint32_t));
        B[i] = (uint32_t *)malloc(n * sizeof(uint32_t));
        C[i] = (uint32_t *)malloc(n * sizeof(uint32_t));
        if (!A[i] || !B[i] || !C[i]) {
            fprintf(stderr, "Error: no se pudo reservar memoria para filas.\n");
            // liberar lo que ya se reservó antes de salir
            for (size_t r = 0; r <= i; r++) {
                free(A[r]); free(B[r]); free(C[r]);
            }
            free(A); free(B); free(C);
            return 1;
        }
    }

    /*Límite de un entero de 32 bits SIN SIGNO = 4.294'967.295

    con un rango de hasta 1036 (0 a 1036)  -> 1036*1036 = 1,073,296

    Con n = 4000
    4000 * 1'073.296 = 4.293'184.000
    */
    // 3. Llenado aleatorio (solo positivos: 0..1036)
    srand((unsigned)time(NULL));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            A[i][j] = (uint32_t)(rand() % 1037);  // 0..1036
            B[i][j] = (uint32_t)(rand() % 1037);  // 0..1036
            C[i][j] = 0u;
        }
    }

    printf("\nEjecutando multiplicacion para N = %zu...\n", n);

    // --- INICIO DE MEDICIONES ---
    cpu_inicio = clock();                             // Inicia tiempo de CPU
    clock_gettime(CLOCK_MONOTONIC, &wall_inicio);      // Inicia tiempo real

    // 4. Algoritmo Secuencial O(n^3)
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            // Usamos acumulador sin signo de 64 bits para evitar overflow intermedio.
            // Luego se almacena en uint32_t (si excede 2^32-1, se trunca módulo 2^32).
            uint64_t sum = 0ULL;
            for (size_t k = 0; k < n; k++) {
                sum += (uint64_t)A[i][k] * (uint64_t)B[k][j];
            }
            C[i][j] = (uint32_t)sum;
        }
    }

    // --- FIN DE MEDICIONES ---
    clock_gettime(CLOCK_MONOTONIC, &wall_fin);         // Termina tiempo real
    cpu_fin = clock();                                 // Termina tiempo de CPU

    // 5. Cálculos de tiempo
    double wall_total = (wall_fin.tv_sec - wall_inicio.tv_sec) +
                        (wall_fin.tv_nsec - wall_inicio.tv_nsec) / 1e9;

    double cpu_total = ((double)(cpu_fin - cpu_inicio)) / CLOCKS_PER_SEC;

    // 6. Resultados
    printf("\n--------------------------------------------\n");
    printf("RESULTADOS DE TIEMPO:\n");
    printf("Wall Clock Time (Real):   %.6f segundos\n", wall_total);
    printf("CPU Time (Procesador):    %.6f segundos\n", cpu_total);
    printf("--------------------------------------------\n");

    // 7. Liberar memoria
    for (size_t i = 0; i < n; i++) {
        free(A[i]); free(B[i]); free(C[i]);
    }
    free(A); free(B); free(C);

    return 0;
}