#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"
export PATH="$HOME/.cargo/bin:$PATH"

cargo build --release >/dev/null

{
    echo "Ejercicio 1 - Verificador de movimientos de caballo"
    echo "Lenguaje: Rust $(rustc --version | awk '{print $2}')"
    echo "Casilla inicial: a2"
    echo "Numero de movimientos n: 500"
    echo "Repeticiones: 10"
    echo "Reloj: std::time::Instant"
    echo
    ./target/release/caballo --bench
} | tee resultados_rust.txt
