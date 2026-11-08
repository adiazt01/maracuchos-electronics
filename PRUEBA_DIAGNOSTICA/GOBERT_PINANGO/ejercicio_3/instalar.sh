#!/usr/bin/env bash
set -euo pipefail

faltantes=()

command -v gcc >/dev/null 2>&1 || faltantes+=("gcc")
command -v make >/dev/null 2>&1 || faltantes+=("make")

if [ "${#faltantes[@]}" -gt 0 ]; then
    echo "Faltan dependencias: ${faltantes[*]}"
    if command -v apt-get >/dev/null 2>&1; then
        echo "Instalando con apt-get..."
        sudo apt-get update
        sudo apt-get install -y build-essential
    else
        echo "Instala manualmente: gcc y make."
        exit 1
    fi
fi

echo "Dependencias verificadas:"
gcc --version | head -n 1
make --version | head -n 1

echo
echo "Compilando el traductor..."
make -C "$(dirname "$0")"
echo "Listo. Ejecuta: ./traductor ejemplo.c"
