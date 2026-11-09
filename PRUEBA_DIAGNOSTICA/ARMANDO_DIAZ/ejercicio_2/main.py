import sys
import time
from pathlib import Path
from typing import TypeAlias

# Declaracion de contratos
Coefficients: TypeAlias = list[int]
Steps: TypeAlias = list[str]


def pascal_row(degree: int) -> Coefficients:
    row: Coefficients = [1]
    for _ in range(degree):
        next_row: Coefficients = [1]
        for index in range(len(row) - 1):
            next_row.append(row[index] + row[index + 1])
        next_row.append(1)
        row = next_row
    return row


def format_term(coefficient: int, exponent: int) -> str:
    if exponent == 0:
        return str(coefficient)
    coefficient_text = "" if coefficient == 1 else str(coefficient)
    power_text = "x" if exponent == 1 else f"x^{exponent}"
    return coefficient_text + power_text


def format_polynomial(coefficients: Coefficients) -> str:
    degree = len(coefficients) - 1
    terms = [
        format_term(coefficients[exponent], exponent)
        for exponent in range(degree, -1, -1)
    ]
    return " + ".join(terms)


def evaluate_step_by_step(coefficients: Coefficients, base_x: int) -> tuple[Steps, int]:
    degree = len(coefficients) - 1
    steps: Steps = []
    total = 0
    for exponent in range(degree, -1, -1):
        term = coefficients[exponent] * base_x ** exponent
        total += term
        steps.append(f"{coefficients[exponent]} * {base_x}^{exponent} = {term}")
    steps.append(f"Suma total: f({base_x}) = {total}")
    return steps, total


def main() -> None:
    degree = int(input("n: "))
    base_x = int(input("x: "))

    coefficients = pascal_row(degree)
    print(f"(x+1)^{degree} = {format_polynomial(coefficients)}")
    print("Coeficientes:", coefficients)

    steps, _ = evaluate_step_by_step(coefficients, base_x)
    print("\nCálculo paso a paso:")
    for step in steps:
        print(step)


REPETITIONS = 10
TIME_DEGREE = 100


def measure_time() -> None:
    """Mide 10 veces generar los coeficientes de (x+1)^100 y convertirlos a texto."""
    here = Path(__file__).parent
    run_times = []
    for _ in range(REPETITIONS):
        start_time = time.perf_counter()
        [str(coefficient) for coefficient in pascal_row(TIME_DEGREE)]
        run_times.append(time.perf_counter() - start_time)

    with open(here / "resultados_python.txt", "w", encoding="utf-8") as output_file:
        output_file.write("Lenguaje: Python\n")
        output_file.write(f"n = {TIME_DEGREE} (generar coeficientes y convertirlos a texto)\n\n")
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
        main()
