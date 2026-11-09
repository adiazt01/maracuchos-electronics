# Ejercicio 2 — Polinomio (x+1)^n con Pascal (Python y C)

## Enunciado

Dado un entero no negativo `n`:

1. Generar los coeficientes de `(x+1)^n` usando el triángulo de Pascal y mostrar el polinomio.
2. Mostrar paso a paso el cálculo de `f(x) = (x+1)^n` para un valor dado de `x`.
3. Implementar el algoritmo utilizando memoria dinámica.
4. Medir el tiempo necesario para generar `n=100` desde Python y desde C.

## Estructura

| Archivo | Descripción |
|---|---|
| `python/polinomio.py` | Implementación en Python |
| `c/polinomio.c` | Implementación en C con GMP |
| `c/Makefile` | Compilación de la versión en C |
| `instalar.sh` | Verificación/instalación de gcc, make, python3 y libgmp-dev |
| `benchmark.sh` | Comparación de tiempos Python vs C |
| `README.md` | Este documento |

## Dependencias

- Python 3.
- GCC y Make.
- GMP (`libgmp-dev`) para enteros de precisión arbitraria en C.

```bash
./instalar.sh
```

## Uso

### Python

```bash
python3 python/polinomio.py <n> <x>
python3 python/polinomio.py 4 2
```

### C

```bash
cd c
make
./polinomio <n> <x>
./polinomio 4 2
```

### Tabla de verificación

```bash
python3 python/polinomio.py --tabla
./c/polinomio --tabla
```

Muestra las filas `n=0..7` del triángulo de Pascal y el polinomio desarrollado.

### Benchmark

```bash
./benchmark.sh [iteraciones] [n]
./benchmark.sh 1000 100
```

También se puede ejecutar cada implementación por separado:

```bash
python3 python/polinomio.py --bench 1000 100
./c/polinomio --bench 1000 100
```

## Algoritmo

La fila `n` del triángulo de Pascal se construye en un arreglo dinámico de `n+1` elementos:

```text
fila[0] = 1
para i = 1..n:
    para j = i..1 (de derecha a izquierda):
        fila[j] = fila[j] + fila[j-1]
```

La actualización se hace de derecha a izquierda para reutilizar el mismo arreglo sin una segunda fila auxiliar.

- **Complejidad temporal:** `O(n^2)`.
- **Memoria:** `O(n)`.
- **Python:** la lista es dinámica y los enteros son de precisión arbitraria nativa.
- **C:** se reserva el arreglo con `malloc`, se inicializan los `mpz_t` y se liberan con `mpz_clear` y `free`. GMP proporciona los enteros de precisión arbitraria.

La evaluación paso a paso calcula cada término `coeficiente * x^potencia`, muestra su valor y el acumulado, y finalmente muestra `f(x)`.

Para la medición del triángulo completo `0..n` se reutiliza el mismo arreglo: se parte de la fila 0 y se aplica la actualización in-place para obtener cada fila siguiente. Así se generan todas las filas con `O(n^2)` de tiempo total y `O(n)` de memoria, sin almacenar el triángulo completo.

## Ejemplo de salida

```text
Coeficientes de (x+1)^4 (fila 4 del triangulo de Pascal):
  1 4 6 4 1
(x+1)^4 = x^4 + 4x^3 + 6x^2 + 4x + 1

Calculo paso a paso de f(2) = (2+1)^4:
  1 * 2^4 = 16    (acumulado: 16)
  4 * 2^3 = 32    (acumulado: 48)
  6 * 2^2 = 24    (acumulado: 72)
  4 * 2^1 = 8    (acumulado: 80)
  1 * 2^0 = 1    (acumulado: 81)
f(2) = 81
```

## Resultados del benchmark

Medición de referencia con 1000 iteraciones y `n=100`. Se reportan dos formas de interpretar «generar n=100»:

1. Generar únicamente la fila `n=100`.
2. Generar las filas `0..100` (triángulo completo) de forma incremental.

| Lenguaje | Medición | Tiempo total | Promedio por generación | Coeficiente/suma de control |
|---|---|---:|---:|---|
| Python | fila `n=100` | 0.7318 s | 731.846 µs | `100891344545564193334812497256` |
| C (GMP) | fila `n=100` | 0.0863 s | 86.346 µs | `100891344545564193334812497256` |
| Python | filas `0..100` | 0.6738 s | 673.799 µs | `5416779509613284387` |
| C (GMP) | filas `0..100` | 0.0842 s | 84.159 µs | `5416779509613284387` |

En esta ejecución, la versión en C resultó aproximadamente 8 veces más rápida que Python en promedio. Los valores dependen del hardware y de la carga del sistema. La suma de control de 64 bits coincide entre Python y C, lo que confirma que ambos generan las mismas filas.

## Validación

- Las salidas de Python y C coinciden para `n=0..7` y para `n=100`.
- El coeficiente central de `(x+1)^100` coincide en ambas implementaciones: `100891344545564193334812497256`.
- Las sumas de control de las filas `0..100` coinciden entre Python y C.
- Se verificó `f(x)` con valores positivos y negativos de `x` (por ejemplo `x=2`, `x=1`, `x=-3`), sin desbordamiento en C gracias a GMP.
