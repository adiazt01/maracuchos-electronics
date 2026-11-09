#!/usr/bin/env bash
set -euo pipefail

if command -v cargo >/dev/null 2>&1 && command -v rustc >/dev/null 2>&1; then
    echo "Rust ya esta instalado:"
    cargo --version
    rustc --version
    exit 0
fi

echo "Instalando Rust mediante rustup..."
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

# shellcheck disable=SC1091
source "$HOME/.cargo/env"

echo "Instalacion completada:"
cargo --version
rustc --version
