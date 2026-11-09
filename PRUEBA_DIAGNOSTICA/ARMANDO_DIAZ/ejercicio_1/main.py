import sys
import time
from pathlib import Path
from typing import TypeAlias

# Declaracion de contratos
GridCoords: TypeAlias = tuple[int,int]
MovementSet: TypeAlias = set[GridCoords]

# 8 posibles saltos del caballo
MOVEMENTS: MovementSet = {
    (1, 2), (2, 1), (-1, 2), (-2, 1),
    (1, -2), (2, -1), (-1, -2), (-2, -1)
}

def get_movement(destination: GridCoords, origin: GridCoords) -> GridCoords:
    return (destination[0] - origin[0], destination[1] - origin[1])

def notation_to_coords(square: str) -> GridCoords:
    column = ord(square[0]) - ord("a")
    row = int(square[1]) - 1
    return (column, row)


def is_knight_move(origin: str, destination: str) -> bool:
    origin_coords = notation_to_coords(origin)
    destination_coords = notation_to_coords(destination)

    movement = get_movement(destination_coords, origin_coords)

    return movement in MOVEMENTS

def check_path(start: str, limit: int, pgn: str) -> str:
    current = start
    moves = pgn.split()[:limit]

    for i, move in enumerate(moves, start=1):
        if not move.startswith("K"):
            return f"INCORRECTO falla letra {move[0]} no pertenece a un caballo."

        if len(move) != 3:
            return f"INCORRECTO falla en el movimiento {i}."

        destination = move[1:]

        if not is_knight_move(current, destination):
            return f"INCORRECTO falla en el movimiento {i}."

        current = destination

    return "movimiento CORRECTO."

REPETITIONS = 10
START_SQUARE = "a2"


def measure_time() -> None:
    """Verifica 10 veces los movimientos de movimientos.txt y guarda los tiempos."""
    here = Path(__file__).parent
    moves = (here / "movimientos.txt").read_text(encoding="utf-8").split()
    joined_moves = " ".join(moves)

    run_times = []
    result = None
    for _ in range(REPETITIONS):
        start_time = time.perf_counter()
        result = check_path(START_SQUARE, len(moves), joined_moves)
        run_times.append(time.perf_counter() - start_time)

    with open(here / "resultados_python.txt", "w", encoding="utf-8") as output_file:
        output_file.write("Lenguaje: Python\n")
        output_file.write(f"Casilla inicial: {START_SQUARE}\n")
        output_file.write(f"Movimientos verificados por corrida: {len(moves)}\n")
        output_file.write(f"Resultado: {result}\n\n")
        for run_number, elapsed in enumerate(run_times, start=1):
            output_file.write(f"Corrida {run_number:2d}: {elapsed:.9f} s\n")
        output_file.write(f"\nPromedio: {sum(run_times) / REPETITIONS:.9f} s\n")
        output_file.write(f"Mínimo:   {min(run_times):.9f} s\n")
        output_file.write(f"Máximo:   {max(run_times):.9f} s\n")

    print(f"Promedio: {sum(run_times) / REPETITIONS:.9f} s (ver resultados_python.txt)")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "tiempo":
        measure_time()
    else:
        start_square = input("Casilla inicial (Ak): ").strip()
        move_count = int(input("Cantidad de movimientos (n): "))
        moves = input("Movimientos (C): ")
        print(check_path(start_square, move_count, moves))
