/*
 * Ejercicio 1 (versión en C): verificar movimientos de un caballo.
 *
 *   ./main          -> pide Ak, n y C, y dice si los movimientos son correctos
 *   ./main tiempo   -> mide 10 verificaciones de movimientos.txt y escribe resultados_c.txt
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPETITIONS 10
#define START_SQUARE "a2"

/* True si (column_delta, row_delta) es una L de caballo: 1x2 o 2x1 en cualquier sentido. */
static int is_knight_jump(int column_delta, int row_delta) {
    int absolute_column_delta = abs(column_delta);
    int absolute_row_delta = abs(row_delta);
    return (absolute_column_delta == 1 && absolute_row_delta == 2)
        || (absolute_column_delta == 2 && absolute_row_delta == 1);
}

/* Revisa los primeros move_count movimientos y escribe el resultado en output. */
static void verify_moves(const char *start_square, int move_count, const char *moves,
                         char *output, size_t output_size) {
    int current_column = start_square[0] - 'a';
    int current_row = start_square[1] - '1';
    char *moves_copy = strdup(moves);
    char *token = strtok(moves_copy, " ");
    int move_number = 0;

    snprintf(output, output_size, "movimiento CORRECTO.");

    while (token != NULL && move_number < move_count) {
        move_number++;
        if (token[0] != 'K') {
            snprintf(output, output_size, "INCORRECTO falla letra %c no pertenece a un caballo.", token[0]);
            break;
        }
        if (strlen(token) != 3 || token[1] < 'a' || token[1] > 'h' || token[2] < '1' || token[2] > '8') {
            snprintf(output, output_size, "INCORRECTO falla en el movimiento %d.", move_number);
            break;
        }
        int new_column = token[1] - 'a';
        int new_row = token[2] - '1';
        if (!is_knight_jump(new_column - current_column, new_row - current_row)) {
            snprintf(output, output_size, "INCORRECTO falla en el movimiento %d.", move_number);
            break;
        }
        current_column = new_column;
        current_row = new_row;
        token = strtok(NULL, " ");
    }
    free(moves_copy);
}

/* ---------- Medición de tiempo ---------- */

/* Lee el archivo en memoria dinámica y cuenta cuántos movimientos hay. */
static char *read_moves(const char *file_path, int *move_count) {
    FILE *input_file = fopen(file_path, "rb");
    if (input_file == NULL) {
        perror(file_path);
        return NULL;
    }
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    rewind(input_file);

    char *buffer = malloc((size_t)file_size + 1);
    if (buffer == NULL) {
        fclose(input_file);
        return NULL;
    }
    size_t bytes_read = fread(buffer, 1, (size_t)file_size, input_file);
    buffer[bytes_read] = '\0';
    fclose(input_file);

    for (char *character = buffer; *character; character++) {
        if (*character == '\n' || *character == '\r' || *character == '\t') *character = ' ';
    }

    int counted_moves = 0;
    char *buffer_copy = strdup(buffer);
    for (char *token = strtok(buffer_copy, " "); token != NULL; token = strtok(NULL, " ")) {
        counted_moves++;
    }
    free(buffer_copy);

    *move_count = counted_moves;
    return buffer;
}

static double now_seconds(void) {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp.tv_sec + timestamp.tv_nsec * 1e-9;
}

static int measure_time(void) {
    int move_count = 0;
    char *moves = read_moves("movimientos.txt", &move_count);
    if (moves == NULL) return 1;

    char output[128];
    double run_times[REPETITIONS];
    double total_time = 0.0, best_time = 1e9, worst_time = 0.0;

    for (int run_index = 0; run_index < REPETITIONS; run_index++) {
        double start_time = now_seconds();
        verify_moves(START_SQUARE, move_count, moves, output, sizeof output);
        double end_time = now_seconds();
        run_times[run_index] = end_time - start_time;
        total_time += run_times[run_index];
        if (run_times[run_index] < best_time) best_time = run_times[run_index];
        if (run_times[run_index] > worst_time) worst_time = run_times[run_index];
    }

    FILE *output_file = fopen("resultados_c.txt", "w");
    if (output_file == NULL) {
        perror("resultados_c.txt");
        free(moves);
        return 1;
    }
    fprintf(output_file, "Lenguaje: C\n");
    fprintf(output_file, "Casilla inicial: %s\n", START_SQUARE);
    fprintf(output_file, "Movimientos verificados por corrida: %d\n", move_count);
    fprintf(output_file, "Resultado: %s\n\n", output);
    for (int run_index = 0; run_index < REPETITIONS; run_index++) {
        fprintf(output_file, "Corrida %2d: %.9f s\n", run_index + 1, run_times[run_index]);
    }
    fprintf(output_file, "\nPromedio: %.9f s\n", total_time / REPETITIONS);
    fprintf(output_file, "Mínimo:   %.9f s\n", best_time);
    fprintf(output_file, "Máximo:   %.9f s\n", worst_time);
    fclose(output_file);

    printf("Promedio: %.9f s (ver resultados_c.txt)\n", total_time / REPETITIONS);
    free(moves);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "tiempo") == 0) {
        return measure_time();
    }

    char start_square[16];
    int move_count;
    char moves[8192];
    char output[128];

    printf("Casilla inicial (Ak): ");
    if (scanf("%15s", start_square) != 1) return 1;
    printf("Cantidad de movimientos (n): ");
    if (scanf("%d", &move_count) != 1) return 1;
    getchar();
    printf("Movimientos (C): ");
    if (fgets(moves, sizeof moves, stdin) == NULL) return 1;
    moves[strcspn(moves, "\n")] = '\0';

    verify_moves(start_square, move_count, moves, output, sizeof output);
    printf("%s\n", output);
    return 0;
}
