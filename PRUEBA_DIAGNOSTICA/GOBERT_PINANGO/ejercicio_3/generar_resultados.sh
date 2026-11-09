#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

make >/dev/null

{
    echo "Ejercicio 3 - Palabras reservadas de C traducidas al espanol"
    echo "Lenguaje: C"
    echo "Archivo analizado: ejemplo.c"
    echo "Palabras reservadas: C89, C99 y C11"
    echo
    ./traductor ejemplo.c
} > resultados_c.txt

./traductor ejemplo.c --traducir > codigo_traducido.txt 2>/dev/null

cat resultados_c.txt
echo
echo "Codigo traducido guardado en codigo_traducido.txt"
