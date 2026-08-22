/* ------------------------------------------------------------------------
 * Universidad del Valle de Guatemala
 * Curso: CC3069 - Computacion Paralela y Distribuida
 * Descripcion: Benchmark de actualizacion de particulas,
 * mide el tiempo de N repeticiones del ciclo paralelo con distintos
 * schedules controlados en tiempo de ejecucion via OMP_SCHEDULE.
 * -------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_PARTICULAS 100000
#define REPETICIONES 1000

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
} Particula;

Particula particulas[NUM_PARTICULAS];

void inicializarParticulas() {
    for (int i = 0; i < NUM_PARTICULAS; i++) {
        particulas[i].x  = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
        particulas[i].y  = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
        particulas[i].vx = ((float) rand() / RAND_MAX) * 0.01f - 0.005f;
        particulas[i].vy = ((float) rand() / RAND_MAX) * 0.01f - 0.005f;
    }
}

int main() {
    omp_set_num_threads(4);
    inicializarParticulas();

    double inicio = omp_get_wtime();

    for (int r = 0; r < REPETICIONES; r++) {
        #pragma omp parallel for schedule(runtime)
        for (int i = 0; i < NUM_PARTICULAS; i++) {
            particulas[i].x += particulas[i].vx;
            particulas[i].y += particulas[i].vy;

            if (particulas[i].x >= 1.0f || particulas[i].x <= -1.0f) {
                particulas[i].vx = -particulas[i].vx;
            }
            if (particulas[i].y >= 1.0f || particulas[i].y <= -1.0f) {
                particulas[i].vy = -particulas[i].vy;
            }
        }
    }

    double fin = omp_get_wtime();
    printf("Tiempo: %f\n", fin - inicio);

    return 0;
}
