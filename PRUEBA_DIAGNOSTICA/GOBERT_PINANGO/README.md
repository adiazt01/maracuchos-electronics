# Prueba diagnóstica — Lenguajes y Compiladores (2026-II)

- **Estudiante:** Gobert Pinango
- **Sección:** 2
- **Video de defensa:** ver [`ENLACE_VIDEO.md`](ENLACE_VIDEO.md) (completar el enlace de Drive).

## Contenido de esta carpeta

```
GOBERT_PINANGO/
├── README.md
├── ENLACE_VIDEO.md
├── instalar.sh
├── generar_resultados.sh
├── ejercicio_1/                 verificador de movimientos de caballo (Rust)
│   ├── Cargo.toml
│   ├── src/main.rs
│   ├── movimientos.txt
│   ├── instalar.sh
│   ├── generar_resultados.sh
│   ├── resultados_rust.txt
│   └── README.md
├── ejercicio_2/
│   ├── python/
│   │   ├── polinomio.py
│   │   └── resultados_python.txt
│   ├── c/
│   │   ├── polinomio.c
│   │   ├── Makefile
│   │   └── resultados_c.txt
│   ├── instalar.sh
│   ├── benchmark.sh
│   ├── generar_resultados.sh
│   └── README.md
└── ejercicio_3/
    ├── traductor.c
    ├── ejemplo.c
    ├── Makefile
    ├── instalar.sh
    ├── generar_resultados.sh
    ├── resultados_c.txt
    ├── codigo_traducido.txt
    └── README.md
```

Cada ejercicio incluye su propio `README.md` con el detalle del algoritmo, la instalación, la ejecución y la validación.

## Ejercicio 1 — Caballo de ajedrez (Rust)

Verifica una secuencia de movimientos `K<columna><fila>` sobre un tablero `a..h × 1..8`. Se detiene en el primer error e informa si falló el movimiento o si la letra no corresponde a un caballo.

```bash
cd ejercicio_1
./instalar.sh
cargo run --release -- a1 4 "Kc2 Kd4 Kb3 Kc5"
cargo run --release -- --bench
```

El benchmark verifica 10 veces la secuencia de 500 movimientos desde `a2`. Los resultados quedan en `resultados_rust.txt`.

## Ejercicio 2 — Triángulo de Pascal y (x+1)^n (Python y C)

Genera los coeficientes de `(x+1)^n` con memoria dinámica, muestra el polinomio y el cálculo paso a paso de `f(x)`, y compara los tiempos de Python contra C con GMP para `n=100`.

```bash
cd ejercicio_2
./instalar.sh

python3 python/polinomio.py 4 2
cd c && make && ./polinomio 4 2 && cd ..

./benchmark.sh 1000 100
```

Los resultados quedan en `python/resultados_python.txt` y `c/resultados_c.txt`.

## Ejercicio 3 — Palabras reservadas de C (C)

Carga un archivo C en memoria dinámica, detecta las palabras reservadas de C89, C99 y C11, las traduce al español y conserva intactos comentarios, cadenas y caracteres.

```bash
cd ejercicio_3
./instalar.sh
./traductor ejemplo.c
./traductor ejemplo.c --traducir > codigo_traducido.txt
```

Los resultados quedan en `resultados_c.txt` y `codigo_traducido.txt`.

## Instalación y reproducción de resultados

Desde esta carpeta:

```bash
./instalar.sh
./generar_resultados.sh
```

`instalar.sh` verifica/instala Rust, Python, GCC, Make y GMP. `generar_resultados.sh` vuelve a ejecutar los benchmarks y regenera los archivos `resultados_*.txt`.
