# Ejercicio 1 — Caballo de ajedrez (Rust)

## Enunciado

Dado un tablero con filas `1..8` y columnas `a..h`, una posición inicial `Ak` de un caballo, un número `n` y una cadena `C` con `n` movimientos, indicar si todos los movimientos son válidos. El programa se detiene en el primer movimiento incorrecto.

Cada movimiento se escribe con la notación `K<columna><fila>`, por ejemplo `Kc2`. La letra `K` indica que la pieza es un caballo.

## Estructura

| Archivo | Descripción |
|---|---|
| `Cargo.toml` | Configuración del proyecto Cargo |
| `src/main.rs` | Implementación del verificador |
| `movimientos.txt` | Secuencia de 500 movimientos del enunciado |
| `instalar.sh` | Instalación de Rust mediante `rustup` |
| `README.md` | Este documento |

## Instalación

```bash
./instalar.sh
```

Si Rust ya está instalado, el script solo muestra las versiones. En caso contrario, instala `rustup` y la cadena estable de Rust en `~/.cargo`.

## Compilación y ejecución

```bash
cargo build --release
./target/release/caballo a1 4 "Kc2 Kd4 Kb3 Kc5"
```

También se puede usar `cargo run`:

```bash
cargo run --release -- a1 4 "Kc2 Kd4 Kb3 Kc5"
```

### Modos de uso

```text
caballo <posicion_inicial> <n> [movimientos...]
caballo --bench
```

- Sin argumentos: ejecuta los 9 ejemplos del enunciado y luego el benchmark.
- `--bench` o `-b`: ejecuta únicamente el benchmark de 500 movimientos.

### Ejemplos

```bash
./target/release/caballo a1 4 "Kc2 Kd4 Kb3 Kc5"
# salida: movimiento CORRECTO.

./target/release/caballo a1 4 "Kc2 Kd4 Kb4 Kc5"
# salida: movimiento INCORRECTO falla en el movimiento 3.

./target/release/caballo a1 10 "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Qb4"
# salida: movimiento INCORRECTO falla letra Q no pertenece a un caballo.

./target/release/caballo a1 10 "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kh6 Kb4"
# salida: movimiento INCORRECTO falla en el movimiento 9.
```

## Algoritmo

1. Se parsea la posición inicial a fila y columna numéricas.
2. Para cada movimiento `i`:
   - Se verifica que el token no esté vacío y que su primera letra sea `K`. Si no lo es, se reporta la letra inválida.
   - Se verifica que el token tenga exactamente 3 caracteres y que la casilla destino esté dentro del tablero.
   - Se calcula la diferencia absoluta de filas y columnas. Un movimiento de caballo válido cumple `(1,2)` o `(2,1)`.
   - Si el movimiento es inválido, se detiene la verificación y se informa el número de movimiento.
3. Si se procesan los `n` movimientos sin errores, la salida es `movimiento CORRECTO.`

La verificación de una secuencia de `n` movimientos es `O(n)` en tiempo y `O(1)` en memoria adicional.

## Benchmark

El benchmark verifica 10 veces la secuencia de 500 movimientos desde `a2`:

```bash
./target/release/caballo --bench
```

Resultado de referencia en el equipo de desarrollo:

```text
=== Verificacion de 500 movimientos desde a2 (10 repeticiones) ===
corrida  1:     3.985 us -> movimiento CORRECTO.
...
corrida 10:     3.294 us -> movimiento CORRECTO.

Tiempo total: 33.672 us
Tiempo promedio por verificacion: 3.367 us
Tiempo minimo: 3.280 us
Mediana: 3.291 us
```

Los tiempos pueden variar según la carga del sistema. Se reportan el promedio, el mínimo y la mediana para reducir el efecto de mediciones atípicas.

## Validación

Se comprobaron los 9 ejemplos del enunciado y la secuencia de 500 movimientos. Todos los resultados coinciden con las salidas esperadas. La secuencia completa se valida como `movimiento CORRECTO.`
