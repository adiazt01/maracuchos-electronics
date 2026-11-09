#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

./ejercicio_1/generar_resultados.sh
./ejercicio_2/generar_resultados.sh
./ejercicio_3/generar_resultados.sh
