#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

ITERACIONES="${1:-1000}"
N="${2:-100}"

make -C c >/dev/null

{
    echo "Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (n=${N})"
    echo "Lenguaje: Python $(python3 --version | awk '{print $2}')"
    echo "Repeticiones: ${ITERACIONES}"
    echo "Reloj: time.perf_counter()"
    echo
    python3 python/polinomio.py --bench "$ITERACIONES" "$N"
} > python/resultados_python.txt

{
    echo "Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (n=${N})"
    echo "Lenguaje: C con GMP"
    echo "Repeticiones: ${ITERACIONES}"
    echo "Reloj: clock_gettime(CLOCK_MONOTONIC)"
    echo
    c/polinomio --bench "$ITERACIONES" "$N"
} > c/resultados_c.txt

cat python/resultados_python.txt
echo
cat c/resultados_c.txt
