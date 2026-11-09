#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

ITERACIONES="${1:-1000}"
N="${2:-100}"

make -C c >/dev/null

echo "=== Comparacion de tiempos: generar la fila n=${N} ==="
echo "Iteraciones por lenguaje: ${ITERACIONES}"
echo

echo "--- Python ---"
python3 python/polinomio.py --bench "$ITERACIONES" "$N"
echo
echo "--- C (GMP) ---"
c/polinomio --bench "$ITERACIONES" "$N"
