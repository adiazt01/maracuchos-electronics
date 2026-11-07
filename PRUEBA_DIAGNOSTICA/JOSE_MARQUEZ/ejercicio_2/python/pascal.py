"""Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (Python).

El usuario escribe un entero no negativo n. El programa lo valida, genera el
triangulo de Pascal, muestra los coeficientes y el polinomio (x+1)^n, y luego el
calculo paso a paso de f(x) = (x+1)^n para un valor fijo de x (X_EVALUACION),
que NO se pide al usuario. Si n = 100 se mide ademas el tiempo de generacion
(resultados_python.txt).

Uso:
    python pascal.py            -> modo interactivo (pide n)
    python pascal.py --pruebas  -> casos de prueba automaticos
"""
import os
import sys
import time
from math import comb

N_MAX = 2000               # el triangulo completo ocupa ~340 MB con n = 2000
X_EVALUACION = 2           # x fijo del calculo paso a paso (parte b)
MOSTRAR_EVALUACION = True  # False = no mostrar la parte b
REPETICIONES = 1000
CARPETA = os.path.dirname(os.path.abspath(__file__))
ARCHIVO_RESULTADOS = os.path.join(CARPETA, "resultados_python.txt")


def generar_triangulo(n):
    """Filas 0..n del triangulo de Pascal.

    Programacion dinamica: cada fila se construye a partir de la anterior
    (fila[k] = previa[k-1] + previa[k]) y las filas ya calculadas se conservan.
    """
    triangulo = [[1]]
    for i in range(1, n + 1):
        previa = triangulo[-1]
        fila = [1] * (i + 1)
        for k in range(1, i):
            fila[k] = previa[k - 1] + previa[k]
        triangulo.append(fila)
    return triangulo


def polinomio_texto(fila):
    """fila[k] = C(n, k) es el coeficiente de x^(n-k). Ej.: [1,2,1] -> 'x^2+2x+1'."""
    n = len(fila) - 1
    terminos = []
    for k, c in enumerate(fila):
        p = n - k
        if p == 0:
            terminos.append(str(c))
        else:
            coef = "" if c == 1 else str(c)
            terminos.append(coef + "x" + ("" if p == 1 else "^" + str(p)))
    return "+".join(terminos)


def evaluar_paso_a_paso(fila, x, mostrar=True):
    """Parte (b): f(x) = suma de C(n,k) * x^(n-k), mostrando cada termino."""
    n = len(fila) - 1
    potencias = [1] * (n + 1)                 # potencias[p] = x^p, tambien por DP
    for p in range(1, n + 1):
        potencias[p] = potencias[p - 1] * x

    acumulado = 0
    for k, c in enumerate(fila):
        p = n - k
        termino = c * potencias[p]
        acumulado += termino
        if mostrar:
            print(f"Paso {k + 1:3d}: {c} * ({x})^{p} = {c} * {potencias[p]} "
                  f"= {termino}   acumulado = {acumulado}")
    return acumulado


def mostrar_evaluacion(fila, x):
    n = len(fila) - 1
    print(f"Calculo paso a paso de f(x) = (x+1)^{n} para x = {x}:")
    resultado = evaluar_paso_a_paso(fila, x)
    directo = (x + 1) ** n
    print(f"\nf({x}) = {resultado}")
    print(f"Verificacion directa (x+1)^n = {directo} -> "
          + ("coincide" if directo == resultado else "NO COINCIDE"))


# ------------------------------------------- validacion y lectura interactiva
def validar_n(texto):
    """n debe ser un entero no negativo escrito solo con digitos.

    Devuelve (n, None) o (None, motivo_del_rechazo).
    """
    t = texto.strip()
    if not t:
        return None, "El campo no puede estar vacio."
    if t.startswith("-"):
        return None, "Debe ser un numero entero no negativo (sin signo negativo)."
    if not (t.isascii() and t.isdigit()):
        return None, f"'{t}' no es un numero entero valido (no use letras ni simbolos)."
    n = int(t)
    if n > N_MAX:
        return None, (f"n supera el maximo ({N_MAX}) que este programa acepta: "
                      "el triangulo completo no cabria en memoria.")
    return n, None


def pedir_n():
    """Pide n hasta que sea valido. None si se cierra la entrada."""
    while True:
        try:
            texto = input("Ingrese un numero entero no negativo n: ")
        except EOFError:
            return None
        valor, error = validar_n(texto)
        if error is None:
            return valor
        print("  " + error)


# ------------------------------------------------------ medicion (n = 100)
def medir_n100():
    """Mide el tiempo de generar los polinomios de n = 0 hasta n = 100."""
    print("\n[Ejecutando prueba de rendimiento para n = 100...]")
    t0 = time.perf_counter()
    for _ in range(REPETICIONES):
        triangulo = generar_triangulo(100)
    t1 = time.perf_counter()
    total = (t1 - t0) * 1000.0
    central = triangulo[100][50]

    lineas = [
        "Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (n = 100)",
        f"Lenguaje: Python {sys.version.split()[0]}",
        "Se generan los polinomios de n = 0 hasta n = 100 (101 filas)",
        f"Repeticiones: {REPETICIONES}",
        "Reloj: time.perf_counter()",
        "",
        f"Tiempo total ({REPETICIONES} repeticiones): {total:.6f} ms",
        f"Tiempo promedio por generacion completa: {total / REPETICIONES:.6f} ms",
        f"Coeficiente central C(100,50) = {central}",
        "",
        "Polinomio (x+1)^100:",
        polinomio_texto(triangulo[100]),
    ]
    with open(ARCHIVO_RESULTADOS, "w", encoding="utf-8") as f:
        f.write("\n".join(lineas) + "\n")
    print(f"Tiempo total ({REPETICIONES} repeticiones para n=100): {total:.6f} ms "
          f"(promedio: {total / REPETICIONES:.6f} ms)")
    print("Resultados guardados en " + ARCHIVO_RESULTADOS)


# ------------------------------------------------------------- pruebas
ESPERADOS = ["1", "x+1", "x^2+2x+1", "x^3+3x^2+3x+1", "x^4+4x^3+6x^2+4x+1",
             "x^5+5x^4+10x^3+10x^2+5x+1", "x^6+6x^5+15x^4+20x^3+15x^2+6x+1",
             "x^7+7x^6+21x^5+35x^4+35x^3+21x^2+7x+1"]
N_VALIDOS = {"0": 0, "5": 5, " 12 ": 12, "100": 100, "2000": 2000, "007": 7}
N_INVALIDOS = ["", "   ", "abc", "-5", "-", "+", "+5", "4.5", "1e3", "5 5", "5a",
               "2001", "99999999999", "\u00b2"]


def correr_pruebas():
    errores = 0
    triangulo = generar_triangulo(100)
    for n, esperado in enumerate(ESPERADOS):          # ejemplos del enunciado
        obtenido = polinomio_texto(triangulo[n])
        ok = obtenido == esperado
        errores += not ok
        print(f"[{'OK ' if ok else 'ERROR'}] (x+1)^{n} = {obtenido}")
    for n, fila in enumerate(triangulo):               # propiedades del triangulo
        if fila != [comb(n, k) for k in range(n + 1)] or sum(fila) != 2 ** n:
            errores += 1
            print(f"[ERROR] fila {n} no coincide con los coeficientes binomiales")
    for n in (0, 1, 5, 10, 30):                         # evaluacion (parte b)
        for x in (-3, -1, 0, 1, 2, 7):
            if evaluar_paso_a_paso(triangulo[n], x, mostrar=False) != (x + 1) ** n:
                errores += 1
                print(f"[ERROR] f({x}) para n = {n}")
    for texto, esperado in N_VALIDOS.items():           # validacion de n
        if validar_n(texto)[0] != esperado:
            errores += 1
            print(f"[ERROR] n valido rechazado: {texto!r}")
    for texto in N_INVALIDOS:
        if validar_n(texto)[0] is not None:
            errores += 1
            print(f"[ERROR] se acepto un n invalido: {texto!r}")
    print(f"\nPruebas: {'todas correctas' if errores == 0 else str(errores) + ' errores'}\n")
    return errores == 0


def main():
    if len(sys.argv) == 2 and sys.argv[1] == "--pruebas":
        correr_pruebas()
        return
    if len(sys.argv) != 1:
        print("Uso: python pascal.py  |  python pascal.py --pruebas")
        return
    n = pedir_n()
    if n is None:
        return
    fila = generar_triangulo(n)[n]
    print(f"\nCoeficientes de (x+1)^{n}: {fila}")
    print(f"\n(x+1)^{n} = {polinomio_texto(fila)}\n")
    if MOSTRAR_EVALUACION:
        mostrar_evaluacion(fila, X_EVALUACION)
    if n == 100:                      # la catedra pide el tiempo para n = 100
        medir_n100()


if __name__ == "__main__":
    main()
