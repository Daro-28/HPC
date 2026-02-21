#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // clock(), time(), struct timespec
#include <errno.h>  // errno (para strtol)

/**
 * Multiplicacion de Matrices Cuadradas
 * Medicion de Wall Clock Time vs CPU Time
 * Uso: ./matmul <N>
 */
int main(int argc, char *argv[]) {
    int n;

    // Variables para Wall Clock (tiempo real)
    struct timespec wall_inicio, wall_fin;
    // Variables para CPU Time (tiempo de procesador)
    clock_t cpu_inicio, cpu_fin;

    // 1. Entrada de datos (SOLO por línea de comandos)
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\nEjemplo: %s 512\n", argv[0], argv[0]);
        return 1;
    }

    // Convertir argv[1] a entero de forma segura (mejor que atoi)
    errno = 0;
    char *endptr = NULL;
    long tmp = strtol(argv[1], &endptr, 10);

    // Validaciones: conversión correcta, sin basura, sin overflow/underflow
    if (errno != 0 || endptr == argv[1] || *endptr != '\0') {
        fprintf(stderr, "Error: N debe ser un entero válido.\n");
        return 1;
    }

    if (tmp <= 0 || tmp > 100000) { // límite arbitrario para evitar valores absurdos
        fprintf(stderr, "Error: N debe ser > 0 y razonable.\n");
        return 1;
    }

    n = (int)tmp;

    // 2. Reserva de memoria
    int **A = (int **)malloc(n * sizeof(int *));
    int **B = (int **)malloc(n * sizeof(int *));
    int **C = (int **)malloc(n * sizeof(int *));
    if (!A || !B || !C) {
        fprintf(stderr, "Error: no se pudo reservar memoria para punteros.\n");
        free(A); free(B); free(C);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        A[i] = (int *)malloc(n * sizeof(int));
        B[i] = (int *)malloc(n * sizeof(int));
        C[i] = (int *)malloc(n * sizeof(int));
        if (!A[i] || !B[i] || !C[i]) {
            fprintf(stderr, "Error: no se pudo reservar memoria para filas.\n");
            // liberar lo que ya se reservó antes de salir
            for (int r = 0; r <= i; r++) {
                free(A[r]); free(B[r]); free(C[r]);
            }
            free(A); free(B); free(C);
            return 1;
        }
    }

    // 3. Llenado aleatorio
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
            C[i][j] = 0;
        }
    }

    printf("\nEjecutando multiplicacion para N = %d...\n", n);

    // --- INICIO DE MEDICIONES ---
    cpu_inicio = clock();                             // Inicia tiempo de CPU
    clock_gettime(CLOCK_MONOTONIC, &wall_inicio);      // Inicia tiempo real

    // 4. Algoritmo Secuencial O(n^3)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
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
    for (int i = 0; i < n; i++) {
        free(A[i]); free(B[i]); free(C[i]);
    }
    free(A); free(B); free(C);

    return 0;
}