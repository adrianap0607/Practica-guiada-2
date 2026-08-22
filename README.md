# Práctica Guiada 02 - CC3069 Computacion Paralela y Distribuida

Este repositorio contiene los ejercicios de scheduling y cláusulas de privacidad de datos en OpenMP, organizados en tres carpetas.

```
.
├── Ejercicio1-1/
│   ├── ejercicio1-1_private.c
│   └── ejercicio1-1_firstprivate.c
├── Ejercicio2-1/
│   ├── ejercicio2-1_static.c
│   └── ejercicio2-1_dynamic.c
└── EjercicioenClase/
    └── particulas_benchmark.c
```

**Requisitos:** GCC con soporte OpenMP (`-fopenmp`).

---

## Ejercicio1-1 — `private` vs `firstprivate`

Caída libre de objetos en distintos planetas. Cada hilo actualiza `v0` con la gravedad de su planeta; el objetivo es ver qué pasa cuando la variable compartida `v0` se privatiza de dos formas distintas.

### Compilar y correr

```powershell
gcc -fopenmp -o ejercicio1_private ejercicio1-1_private.c
./ejercicio1_private

gcc -fopenmp -o ejercicio1_firstprivate ejercicio1-1_firstprivate.c
./ejercicio1_firstprivate
```

### Resultados obtenidos

| Clausula | Planeta 0 (gravedad 9.8) | Planeta 8 (gravedad 274.0) |
|---|---|---|
| `private(v0)` | 49.00 | 1370.00 |
| `firstprivate(v0)` | 64.50 | 1385.50 |

### Conclusión

- `private(v0)` crea una copia de `v0` por hilo **sin inicializarla**, así que cada hilo arranca de un valor indeterminado (en esta corrida, 0 en los 10 hilos, pero no está garantizado). El resultado impreso es simplemente `gravedad[i] × tiempo`, sin el aporte de la velocidad inicial de 15.5.
- `firstprivate(v0)` crea una copia por hilo **inicializada con el valor que tenía `v0` justo antes del `parallel for`** (15.5), por lo que cada hilo parte del punto correcto: `15.5 + gravedad[i] × tiempo`.
- **Conclusión:** `firstprivate` es la cláusula correcta cuando cada hilo necesita partir del valor previo de la variable; `private` solo garantiza una copia independiente por hilo, pero no preserva el valor anterior.

---

## Ejercicio2-1 — `schedule(static)` vs `schedule(dynamic, 100)`

Conteo de números primos hasta 20,000,000, con reducción `+:primos`. El costo de `es_primo(n)` crece con `n` (el ciclo llega hasta `sqrt(n)`), por lo que la carga de trabajo es desbalanceada de forma predecible.

### Compilar y correr

```powershell
gcc -fopenmp -O2 -o ejercicio2_static ejercicio2-1_static.c
gcc -fopenmp -O2 -o ejercicio2_dynamic ejercicio2-1_dynamic.c

$env:OMP_NUM_THREADS=4
./ejercicio2_static
./ejercicio2_dynamic
```

### Resultados obtenidos (1,270,607 primos encontrados en todos los casos)

| Número de hilos | t schedule(static) | t schedule(dynamic) | Diferencia (static − dynamic) |
|---|---|---|---|
| 1 | 13.719 s | 13.705 s | 0.014 s |
| 2 | 8.877 s | 6.962 s | 1.915 s |
| 4 | 4.949 s | 3.596 s | 1.353 s |
| 8 | 2.672 s | 2.001 s | 0.671 s |


---

## EjercicioenClase — Benchmark de partículas (`schedule(runtime)`)

Continuación de `particulas.c` (simulación OpenMP + OpenGL). Se extrajo únicamente el cálculo de actualización de posiciones (sin dibujado ni ventana), con 100,000 partículas, 4 hilos fijos (`omp_set_num_threads(4)`) y 1,000 repeticiones del ciclo dentro de la región medida con `omp_get_wtime()`.

El schedule se controla en tiempo de ejecución con `schedule(runtime)` y la variable de entorno `OMP_SCHEDULE`, sin recompilar entre pruebas.

### Compilar y correr

```powershell
gcc -fopenmp -O2 -o particulas_benchmark particulas_benchmark.c

$env:OMP_NUM_THREADS=4
$env:OMP_SCHEDULE="static"       # o "dynamic", "guided", "static,100", "dynamic,100", "guided,100"
./particulas_benchmark
```

### Resultados obtenidos (promedio de 3 corridas)

| Schedule | Chunk | Hilos | Partículas | Tiempo 1 | Tiempo 2 | Tiempo 3 | Tiempo promedio |
|---|---|---|---|---|---|---|---|
| static | default | 4 | 100000 | 0.200 | 0.166 | 0.150 | **0.172** |
| dynamic | default | 4 | 100000 | 9.092 | 9.418 | 8.975 | **9.162** |
| guided | default | 4 | 100000 | 0.132 | 0.149 | 0.126 | **0.136** |
| static | 100 | 4 | 100000 | 0.232 | 0.207 | 0.202 | **0.214** |
| dynamic | 100 | 4 | 100000 | 0.244 | 0.234 | 0.233 | **0.237** |
| guided | 100 | 4 | 100000 | 0.133 | 0.128 | 0.130 | **0.130** |
