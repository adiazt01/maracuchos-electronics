#!/usr/bin/env bash
# Instala lo necesario para el ejercicio 1: Python 3, GCC y make.
set -e

faltan=()
command -v python3 >/dev/null || faltan+=(python3)
command -v gcc >/dev/null || faltan+=(gcc)
command -v make >/dev/null || faltan+=(make)

if [ ${#faltan[@]} -eq 0 ]; then
    echo "Todo ya está instalado."
else
    echo "Instalando: ${faltan[*]}"
    sudo apt-get update
    sudo apt-get install -y "${faltan[@]}"
fi

python3 --version
gcc --version | head -1
make --version | head -1
