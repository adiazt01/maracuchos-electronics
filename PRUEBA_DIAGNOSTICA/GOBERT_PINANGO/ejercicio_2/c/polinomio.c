#define _POSIX_C_SOURCE 199309L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <gmp.h>

static double ahora(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static mpz_t *generar_fila(unsigned long n) {
    mpz_t *fila = malloc((n + 1) * sizeof(mpz_t));
    if (fila == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (unsigned long i = 0; i <= n; i++) {
        mpz_init_set_ui(fila[i], 0);
    }
    mpz_set_ui(fila[0], 1);

    for (unsigned long i = 1; i <= n; i++) {
        for (unsigned long j = i; j > 0; j--) {
            mpz_add(fila[j], fila[j], fila[j - 1]);
        }
    }

    return fila;
}

static void liberar_fila(mpz_t *fila, unsigned long n) {
    for (unsigned long i = 0; i <= n; i++) {
        mpz_clear(fila[i]);
    }
    free(fila);
}

static unsigned long generar_todas_las_filas(unsigned long n) {
    mpz_t *fila = malloc((n + 1) * sizeof(mpz_t));
    if (fila == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (unsigned long i = 0; i <= n; i++) {
        mpz_init_set_ui(fila[i], 0);
    }
    mpz_set_ui(fila[0], 1);

    unsigned long checksum = 0;
    for (unsigned long i = 0; i <= n; i++) {
        if (i > 0) {
            for (unsigned long j = i; j > 0; j--) {
                mpz_add(fila[j], fila[j], fila[j - 1]);
            }
        }
        checksum += mpz_get_ui(fila[i / 2]);
    }

    liberar_fila(fila, n);
    return checksum;
}

static void imprimir_termino(const mpz_t coeficiente, unsigned long potencia,
                             int primero) {
    mpz_t absoluto;
    mpz_init(absoluto);
    mpz_abs(absoluto, coeficiente);

    if (mpz_sgn(coeficiente) < 0) {
        printf(" - ");
    } else if (!primero) {
        printf(" + ");
    }

    if (potencia == 0) {
        gmp_printf("%Zd", absoluto);
    } else {
        if (mpz_cmp_ui(absoluto, 1) != 0) {
            gmp_printf("%Zd", absoluto);
        }
        printf("x");
        if (potencia > 1) {
            printf("^%lu", potencia);
        }
    }

    mpz_clear(absoluto);
}

static void mostrar_polinomio(unsigned long n, mpz_t *fila) {
    printf("Coeficientes de (x+1)^%lu (fila %lu del triangulo de Pascal):\n  ",
           n, n);
    for (unsigned long i = 0; i <= n; i++) {
        gmp_printf("%Zd ", fila[i]);
    }

    printf("\n(x+1)^%lu = ", n);
    int primero = 1;
    for (long potencia = (long)n; potencia >= 0; potencia--) {
        if (mpz_sgn(fila[potencia]) != 0) {
            imprimir_termino(fila[potencia], (unsigned long)potencia, primero);
            primero = 0;
        }
    }
    printf("\n");
}

static void evaluar_pasos(mpz_t *fila, unsigned long n, const mpz_t x) {
    mpz_t total, potencia, termino;
    mpz_inits(total, potencia, termino, NULL);
    mpz_set_ui(total, 0);

    printf("\nCalculo paso a paso de f(");
    mpz_out_str(stdout, 10, x);
    printf(") = (");
    mpz_out_str(stdout, 10, x);
    printf("+1)^%lu:\n", n);

    for (long p = (long)n; p >= 0; p--) {
        mpz_pow_ui(potencia, x, (unsigned long)p);
        mpz_mul(termino, fila[p], potencia);
        mpz_add(total, total, termino);

        printf("  ");
        mpz_out_str(stdout, 10, fila[p]);
        printf(" * ");
        mpz_out_str(stdout, 10, x);
        printf("^%ld = ", p);
        mpz_out_str(stdout, 10, termino);
        printf("    (acumulado: ");
        mpz_out_str(stdout, 10, total);
        printf(")\n");
    }

    printf("f(");
    mpz_out_str(stdout, 10, x);
    printf(") = ");
    mpz_out_str(stdout, 10, total);
    printf("\n");

    mpz_clears(total, potencia, termino, NULL);
}

static void mostrar_tabla(void) {
    for (unsigned long n = 0; n <= 7; n++) {
        mpz_t *fila = generar_fila(n);
        mostrar_polinomio(n, fila);
        liberar_fila(fila, n);
    }
}

static void benchmark(unsigned long iteraciones, unsigned long n) {
    volatile unsigned long checksum = 0;

    double inicio = ahora();
    for (unsigned long k = 0; k < iteraciones; k++) {
        mpz_t *fila = generar_fila(n);
        checksum += mpz_get_ui(fila[n / 2]);
        liberar_fila(fila, n);
    }
    double total = ahora() - inicio;

    double inicio_una = ahora();
    mpz_t *fila = generar_fila(n);
    double tiempo_una = ahora() - inicio_una;

    printf("C (GMP) - generacion de la fila n=%lu\n", n);
    printf("  Iteraciones: %lu\n", iteraciones);
    printf("  Tiempo total: %.6f s\n", total);
    printf("  Promedio por generacion: %.3f us\n",
           total / (double)iteraciones * 1e6);
    printf("  Una sola generacion: %.3f us\n", tiempo_una * 1e6);
    printf("  Coeficiente central: ");
    mpz_out_str(stdout, 10, fila[n / 2]);
    printf("\n  Checksum: %lu\n", checksum);

    liberar_fila(fila, n);

    unsigned long checksum_triangulo = 0;
    double inicio_triangulo = ahora();
    for (unsigned long k = 0; k < iteraciones; k++) {
        checksum_triangulo += generar_todas_las_filas(n);
    }
    double total_triangulo = ahora() - inicio_triangulo;

    double inicio_una_triangulo = ahora();
    checksum_triangulo += generar_todas_las_filas(n);
    double tiempo_una_triangulo = ahora() - inicio_una_triangulo;

    printf("\nC (GMP) - generacion de las filas 0..%lu (triangulo completo)\n",
           n);
    printf("  Iteraciones: %lu\n", iteraciones);
    printf("  Tiempo total: %.6f s\n", total_triangulo);
    printf("  Promedio por generacion: %.3f us\n",
           total_triangulo / (double)iteraciones * 1e6);
    printf("  Una sola generacion: %.3f us\n", tiempo_una_triangulo * 1e6);
    printf("  Suma de control: %lu\n", checksum_triangulo);
}

static void uso(const char *programa) {
    printf("Uso:\n");
    printf("  %s <n> <x>\n", programa);
    printf("  %s --tabla\n", programa);
    printf("  %s --bench [iteraciones] [n]\n", programa);
    printf("Ejemplo:\n");
    printf("  %s 4 2\n", programa);
}

static int parsear_ulong(const char *texto, unsigned long *valor) {
    char *fin = NULL;
    errno = 0;
    unsigned long numero = strtoul(texto, &fin, 10);
    if (errno != 0 || fin == texto || *fin != '\0') {
        return 0;
    }
    *valor = numero;
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        uso(argv[0]);
        return 2;
    }

    if (strcmp(argv[1], "--tabla") == 0) {
        mostrar_tabla();
        return 0;
    }

    if (strcmp(argv[1], "--bench") == 0) {
        unsigned long iteraciones = 1000;
        unsigned long n = 100;

        if (argc > 2 && !parsear_ulong(argv[2], &iteraciones)) {
            fprintf(stderr, "Numero de iteraciones invalido.\n");
            return 2;
        }
        if (argc > 3 && !parsear_ulong(argv[3], &n)) {
            fprintf(stderr, "Valor de n invalido.\n");
            return 2;
        }
        if (iteraciones == 0) {
            fprintf(stderr, "Las iteraciones deben ser mayores que cero.\n");
            return 2;
        }

        benchmark(iteraciones, n);
        return 0;
    }

    if (argc != 3) {
        uso(argv[0]);
        return 2;
    }

    unsigned long n;
    if (!parsear_ulong(argv[1], &n)) {
        fprintf(stderr, "n debe ser un entero no negativo.\n");
        return 2;
    }

    mpz_t x;
    mpz_init(x);
    if (mpz_set_str(x, argv[2], 10) != 0) {
        fprintf(stderr, "x debe ser un numero entero.\n");
        mpz_clear(x);
        return 2;
    }

    mpz_t *fila = generar_fila(n);
    mostrar_polinomio(n, fila);
    evaluar_pasos(fila, n, x);

    liberar_fila(fila, n);
    mpz_clear(x);
    return 0;
}
