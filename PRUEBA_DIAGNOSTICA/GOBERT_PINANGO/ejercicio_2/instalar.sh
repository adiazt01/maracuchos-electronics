#!/usr/bin/env bash
set -euo pipefail

faltantes=()

command -v gcc >/dev/null 2>&1 || faltantes+=("gcc")
command -v make >/dev/null 2>&1 || faltantes+=("make")
command -v python3 >/dev/null 2>&1 || faltantes+=("python3")

if ! pkg-config --exists gmp 2>/dev/null; then
    if [ ! -f /usr/include/gmp.h ] && [ ! -f /usr/include/x86_64-linux-gnu/gmp.h ]; then
        faltantes+=("libgmp-dev")
    fi
fi

if [ "${#faltantes[@]}" -gt 0 ]; then
    echo "Faltan dependencias: ${faltantes[*]}"
    if command -v apt-get >/dev/null 2>&1; then
        echo "Instalando con apt-get..."
        sudo apt-get update
        sudo apt-get install -y build-essential python3 libgmp-dev
    else
        echo "Instala manualmente: gcc, make, python3 y la libreria GMP (libgmp-dev)."
        exit 1
    fi
fi

echo "Dependencias verificadas:"
gcc --version | head -n 1
make --version | head -n 1
python3 --version
echo "GMP: $(pkg-config --modversion gmp 2>/dev/null || echo instalada)"

echo
echo "Compilando la version en C..."
make -C "$(dirname "$0")/c"
echo "Listo. Ejecuta: ./benchmark.sh"
