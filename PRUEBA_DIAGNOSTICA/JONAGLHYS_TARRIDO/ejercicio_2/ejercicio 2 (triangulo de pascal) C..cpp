#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

long long* generar_coeficientes_pascal(int n, long long** coefs) {
    *coefs = (long long*)malloc((n + 1) * sizeof(long long));
    long long* fila_actual = (long long*)malloc((n + 1) * sizeof(long long));
    
    fila_actual[0] = 1;
    for (int i = 1; i <= n; i++) {
        fila_actual[i] = 1;
        for (int j = i - 1; j > 0; j--) {
            fila_actual[j] = fila_actual[j] + fila_actual[j - 1];
        }
    }
    for (int i = 0; i <= n; i++) {
        (*coefs)[i] = fila_actual[i];
    }
    free(fila_actual);
    return *coefs;
}

void evaluar_polinomio(long long* coefs, int n, double x) {
    printf("\n--- Evaluacion paso a paso para x = %.1f de (x+1)^%d ---\n", x, n);
    double total = 0;
    for (int i = 0; i <= n; i++) {
        int pot = n - i;
        double term = coefs[i] * pow(x, pot);
        total += term;
        printf("Termino %d: %lld * (%.1f^%d) = %.4f\n", i + 1, coefs[i], x, pot, term);
    }
    printf("Resultado final f(x) = %.4f\n\n", total);
}

int main() {
    int n_ej = 5;
    long long* coefs = NULL;
    
    // Medición para n=100
    clock_t ini = clock();
    for (int i = 0; i < 100; i++) {
        generar_coeficientes_pascal(100, &coefs);
        free(coefs);
    }
    clock_t fin = clock();
    printf("Tiempo C (n=100): %f segundos\n", ((double)(fin - ini)) / CLOCKS_PER_SEC);

    generar_coeficientes_pascal(n_ej, &coefs);
    evaluar_polinomio(coefs, n_ej, 2.0);
    free(coefs);
    return 0;
}