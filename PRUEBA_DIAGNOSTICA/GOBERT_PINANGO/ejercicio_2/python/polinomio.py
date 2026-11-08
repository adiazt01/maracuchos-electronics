#!/usr/bin/env python3
"""Genera (x+1)^n con el triangulo de Pascal y evalua f(x) paso a paso.

La fila de Pascal se construye en una lista dinamica (memoria dinamica
gestionada por Python) y se actualiza en el mismo arreglo, de derecha a
izquierda, con complejidad O(n^2) y O(n) de memoria.
"""

from __future__ import annotations

import sys
import time


def fila_pascal(n: int) -> list[int]:
    """Devuelve los coeficientes de (x+1)^n usando una fila dinamica."""
    fila = [0] * (n + 1)
    fila[0] = 1
    for i in range(1, n + 1):
        for j in range(i, 0, -1):
            fila[j] += fila[j - 1]
    return fila


def generar_todas_las_filas(n: int) -> int:
    """Genera las filas 0..n de forma incremental y devuelve una suma de control.

    Solo se conserva una fila en memoria; cada fila se obtiene de la anterior
    actualizando el mismo arreglo de derecha a izquierda.
    """
    fila = [0] * (n + 1)
    fila[0] = 1
    checksum = 0

    mascara = (1 << 64) - 1
    for i in range(n + 1):
        if i > 0:
            for j in range(i, 0, -1):
                fila[j] += fila[j - 1]
        checksum = (checksum + fila[i // 2]) & mascara

    return checksum


def formatear_termino(coeficiente: int, potencia: int) -> str:
    if potencia == 0:
        return str(coeficiente)

    base = "x" if potencia == 1 else f"x^{potencia}"
    if coeficiente == 1:
        return base
    if coeficiente == -1:
        return f"-{base}"
    return f"{coeficiente}{base}"


def formatear_polinomio(coeficientes: list[int]) -> str:
    terminos: list[str] = []
    for potencia in range(len(coeficientes) - 1, -1, -1):
        coeficiente = coeficientes[potencia]
        if coeficiente != 0:
            terminos.append(formatear_termino(coeficiente, potencia))

    if not terminos:
        return "0"

    resultado = terminos[0]
    for termino in terminos[1:]:
        if termino.startswith("-"):
            resultado += f" - {termino[1:]}"
        else:
            resultado += f" + {termino}"
    return resultado


def mostrar_polinomio(n: int, coeficientes: list[int]) -> None:
    print(f"Coeficientes de (x+1)^{n} (fila {n} del triangulo de Pascal):")
    print("  " + " ".join(str(c) for c in coeficientes))
    print(f"(x+1)^{n} = {formatear_polinomio(coeficientes)}")


def evaluar_pasos(coeficientes: list[int], x: int) -> int:
    n = len(coeficientes) - 1
    print(f"\nCalculo paso a paso de f({x}) = ({x}+1)^{n}:")

    total = 0
    for potencia in range(n, -1, -1):
        coeficiente = coeficientes[potencia]
        valor_termino = coeficiente * (x**potencia)
        total += valor_termino
        print(
            f"  {coeficiente} * {x}^{potencia} = {valor_termino}"
            f"    (acumulado: {total})"
        )

    print(f"f({x}) = {total}")
    return total


def benchmark(iteraciones: int, n: int) -> None:
    inicio = time.perf_counter()
    for _ in range(iteraciones):
        fila_pascal(n)
    fin = time.perf_counter()

    total = fin - inicio
    promedio = total / iteraciones

    inicio_una = time.perf_counter()
    fila = fila_pascal(n)
    tiempo_una = time.perf_counter() - inicio_una

    print(f"Python - generacion de la fila n={n}")
    print(f"  Iteraciones: {iteraciones}")
    print(f"  Tiempo total: {total:.6f} s")
    print(f"  Promedio por generacion: {promedio * 1e6:.3f} us")
    print(f"  Una sola generacion: {tiempo_una * 1e6:.3f} us")
    print(f"  Coeficiente central: {fila[n // 2]}")

    inicio_triangulo = time.perf_counter()
    checksum = 0
    mascara = (1 << 64) - 1
    for _ in range(iteraciones):
        checksum = (checksum + generar_todas_las_filas(n)) & mascara
    fin_triangulo = time.perf_counter()

    total_triangulo = fin_triangulo - inicio_triangulo
    promedio_triangulo = total_triangulo / iteraciones

    inicio_una_triangulo = time.perf_counter()
    checksum = (checksum + generar_todas_las_filas(n)) & mascara
    tiempo_una_triangulo = time.perf_counter() - inicio_una_triangulo

    print(f"\nPython - generacion de las filas 0..{n} (triangulo completo)")
    print(f"  Iteraciones: {iteraciones}")
    print(f"  Tiempo total: {total_triangulo:.6f} s")
    print(f"  Promedio por generacion: {promedio_triangulo * 1e6:.3f} us")
    print(f"  Una sola generacion: {tiempo_una_triangulo * 1e6:.3f} us")
    print(f"  Suma de control: {checksum}")


def mostrar_tabla() -> None:
    for n in range(8):
        coeficientes = fila_pascal(n)
        print(f"(x+1)^{n} = {formatear_polinomio(coeficientes)}")
        print("  " + " ".join(str(c) for c in coeficientes))


def uso() -> None:
    print("Uso:")
    print("  python3 polinomio.py <n> <x>")
    print("  python3 polinomio.py --tabla")
    print("  python3 polinomio.py --bench [iteraciones] [n]")
    print("Ejemplo:")
    print('  python3 polinomio.py 4 2')


def main() -> int:
    argumentos = sys.argv[1:]

    if not argumentos:
        uso()
        return 2

    if argumentos[0] == "--tabla":
        mostrar_tabla()
        return 0

    if argumentos[0] == "--bench":
        iteraciones = int(argumentos[1]) if len(argumentos) > 1 else 1000
        n = int(argumentos[2]) if len(argumentos) > 2 else 100
        if iteraciones <= 0 or n < 0:
            print("Los valores deben ser enteros positivos (n >= 0).", file=sys.stderr)
            return 2
        benchmark(iteraciones, n)
        return 0

    if len(argumentos) != 2:
        uso()
        return 2

    try:
        n = int(argumentos[0])
        x = int(argumentos[1])
    except ValueError:
        print("n y x deben ser numeros enteros.", file=sys.stderr)
        return 2

    if n < 0:
        print("n debe ser un entero no negativo.", file=sys.stderr)
        return 2

    coeficientes = fila_pascal(n)
    mostrar_polinomio(n, coeficientes)
    evaluar_pasos(coeficientes, x)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
