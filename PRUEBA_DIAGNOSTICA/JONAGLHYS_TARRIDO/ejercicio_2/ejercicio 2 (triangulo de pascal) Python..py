import time

def generar_coeficientes_pascal(n):
    fila = [1]
    for _ in range(n):
        fila = [1] + [fila[i] + fila[i+1] for i in range(len(fila)-1)] + [1]
    return fila

def evaluar_polinomio(coefs, n, x):
    print(f"\n--- Evaluacion paso a paso para x = {x} de (x+1)^{n} ---")
    total = 0
    for i, coef in enumerate(coefs):
        pot = n - i
        term = coef * (x ** pot)
        total += term
        print(f"Termino {i+1}: {coef} * ({x}^{pot}) = {term}")
    print(f"Resultado final f(x) = {total}\n")

# Medición para n=100
inicio = time.time()
for _ in range(100):
    generar_coeficientes_pascal(100)
fin = time.time()
print(f"Tiempo Python (n=100): {fin - inicio:.6f} segundos")

# Demostración n=5
coefs = generar_coeficientes_pascal(5)
evaluar_polinomio(coefs, 5, 2.0)
