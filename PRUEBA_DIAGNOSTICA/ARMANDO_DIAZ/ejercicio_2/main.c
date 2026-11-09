/*
 * Ejercicio 2 (versión en C): triángulo de Pascal, polinomio (x+1)^n y evaluación paso a paso.
 *
 *   ./main          -> pide n y x, muestra el polinomio y el cálculo paso a paso
 *   ./main tiempo   -> mide 10 veces n=100 y escribe resultados_c.txt
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned __int128 big_number;

#define REPETITIONS 10
#define TIME_DEGREE 100

/* Memoria dinámica: devuelve [C(degree,0), ..., C(degree,degree)], los coeficientes de (x+1)^degree. */
static big_number *pascal_row(int degree) {
    big_number *row = calloc((size_t)degree + 1, sizeof *row);
    if (row == NULL) return NULL;
    row[0] = 1;
    for (int current_row = 1; current_row <= degree; current_row++) {
        for (int column = current_row; column >= 1; column--) {
            row[column] += row[column - 1];
        }
    }
    return row;
}

/* Convierte un número de 128 bits a texto (no existe printf para big_number). */
static void to_text(big_number value, char *output_text, size_t output_size) {
    char digits[64];
    int digit_count = 0;
    if (value == 0) {
        snprintf(output_text, output_size, "0");
        return;
    }
    while (value > 0) {
        digits[digit_count++] = (char)('0' + (int)(value % 10));
        value /= 10;
    }
    size_t write_index = 0;
    while (digit_count > 0 && write_index + 1 < output_size) {
        output_text[write_index++] = digits[--digit_count];
    }
    output_text[write_index] = '\0';
}

static big_number power(big_number base, int exponent) {
    big_number result = 1;
    for (int multiply_index = 0; multiply_index < exponent; multiply_index++) {
        result *= base;
    }
    return result;
}

static void print_polynomial(const big_number *coefficients, int degree) {
    char coefficient_text[64];
    printf("(x+1)^%d = ", degree);
    for (int exponent = degree; exponent >= 0; exponent--) {
        if (exponent != degree) printf(" + ");
        if (exponent == 0) {
            to_text(coefficients[exponent], coefficient_text, sizeof coefficient_text);
            printf("%s", coefficient_text);
        } else {
            if (coefficients[exponent] != 1) {
                to_text(coefficients[exponent], coefficient_text, sizeof coefficient_text);
                printf("%s", coefficient_text);
            }
            if (exponent == 1) printf("x");
            else printf("x^%d", exponent);
        }
    }
    printf("\n");
}

static void show_steps(const big_number *coefficients, int degree, unsigned long long base_x) {
    char coefficient_text[64], term_text[64], total_text[64];
    big_number total = 0;
    printf("\nCálculo paso a paso:\n");
    for (int exponent = degree; exponent >= 0; exponent--) {
        big_number term = coefficients[exponent] * power((big_number)base_x, exponent);
        total += term;
        to_text(coefficients[exponent], coefficient_text, sizeof coefficient_text);
        to_text(term, term_text, sizeof term_text);
        printf("%s * %llu^%d = %s\n", coefficient_text, base_x, exponent, term_text);
    }
    to_text(total, total_text, sizeof total_text);
    printf("Suma total: f(%llu) = %s\n", base_x, total_text);
}

/* ---------- Medición de tiempo ---------- */

static double now_seconds(void) {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp.tv_sec + timestamp.tv_nsec * 1e-9;
}

static int measure_time(void) {
    double run_times[REPETITIONS];
    double total_time = 0.0, best_time = 1e9, worst_time = 0.0;
    char text_buffer[64];

    for (int run_index = 0; run_index < REPETITIONS; run_index++) {
        double start_time = now_seconds();
        big_number *row = pascal_row(TIME_DEGREE);
        if (row == NULL) return 1;
        for (int coefficient_index = 0; coefficient_index <= TIME_DEGREE; coefficient_index++) {
            to_text(row[coefficient_index], text_buffer, sizeof text_buffer);
        }
        free(row);
        double end_time = now_seconds();
        run_times[run_index] = end_time - start_time;
        total_time += run_times[run_index];
        if (run_times[run_index] < best_time) best_time = run_times[run_index];
        if (run_times[run_index] > worst_time) worst_time = run_times[run_index];
    }

    FILE *output_file = fopen("resultados_c.txt", "w");
    if (output_file == NULL) {
        perror("resultados_c.txt");
        return 1;
    }
    fprintf(output_file, "Lenguaje: C\n");
    fprintf(output_file, "n = %d (generar coeficientes y convertirlos a texto)\n\n", TIME_DEGREE);
    for (int run_index = 0; run_index < REPETITIONS; run_index++) {
        fprintf(output_file, "Corrida %2d: %.9f s\n", run_index + 1, run_times[run_index]);
    }
    fprintf(output_file, "\nPromedio: %.9f s\n", total_time / REPETITIONS);
    fprintf(output_file, "Mínimo:   %.9f s\n", best_time);
    fprintf(output_file, "Máximo:   %.9f s\n", worst_time);
    fclose(output_file);

    printf("Promedio: %.9f s (ver resultados_c.txt)\n", total_time / REPETITIONS);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "tiempo") == 0) return measure_time();

    int degree;
    unsigned long long base_x;
    printf("n: ");
    if (scanf("%d", &degree) != 1 || degree < 0) return 1;
    printf("x: ");
    if (scanf("%llu", &base_x) != 1) return 1;

    big_number *row = pascal_row(degree);
    if (row == NULL) return 1;
    print_polynomial(row, degree);
    show_steps(row, degree, base_x);
    free(row);
    return 0;
}
