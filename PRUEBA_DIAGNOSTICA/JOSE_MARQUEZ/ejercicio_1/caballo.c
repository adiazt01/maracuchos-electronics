/* Ejercicio 1 - Verificador de movimientos de caballo (C)
 *
 * El programa pide: la posicion inicial Ak, el numero de movimientos n y la cadena C.
 * Verifica los movimientos (se detiene en el primer error), muestra el resultado y
 * luego mide el tiempo de repetir esa verificacion 10 veces, escribiendolo en un .txt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define REPETICIONES 10
#define ARCHIVO_RESULTADOS "resultados_c.txt"
#define MAX_LINEA 65536

typedef struct {
    int correcto;      /* 1 si todos los movimientos son validos */
    int movimiento;    /* numero (desde 1) del primer movimiento invalido */
    char letra;        /* letra de pieza invalida, o 0 si el fallo es de movimiento */
} Resultado;

/* ------------------------------------------------------------- logica */

/* "c2" -> columna 0..7 y fila 0..7. Devuelve 0 si no es una casilla valida. */
static int parsear_casilla(const char *s, int *col, int *fila) {
    if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8') return 0;
    *col = s[0] - 'a';
    *fila = s[1] - '1';
    return 1;
}

/* El caballo se mueve 2 casillas en una direccion y 1 en la otra (forma de L). */
static int es_salto_de_caballo(int c1, int f1, int c2, int f2) {
    int dc = abs(c1 - c2), df = abs(f1 - f2);
    return (dc == 1 && df == 2) || (dc == 2 && df == 1);
}

/* Recorre la cadena sin modificarla y se detiene en el primer error. */
static Resultado verificar(const char *inicio, const char *cadena) {
    Resultado r = {1, 0, 0};
    int col, fila, ncol, nfila, numero = 0;
    const char *p = cadena;

    if (!parsear_casilla(inicio, &col, &fila)) {   /* no deberia ocurrir: se valida antes */
        r.correcto = 0;
        return r;
    }
    while (*p) {
        const char *ini;
        size_t largo;

        while (isspace((unsigned char)*p)) p++;   /* saltar espacios y saltos de linea */
        if (!*p) break;
        ini = p;
        while (*p && !isspace((unsigned char)*p)) p++;
        largo = (size_t)(p - ini);
        numero++;

        if (ini[0] != 'K') {                      /* la pieza debe ser un caballo */
            r.correcto = 0; r.movimiento = numero; r.letra = ini[0];
            return r;
        }
        if (largo != 3 || !parsear_casilla(ini + 1, &ncol, &nfila) ||
            !es_salto_de_caballo(col, fila, ncol, nfila)) {
            r.correcto = 0; r.movimiento = numero;
            return r;
        }
        col = ncol; fila = nfila;                 /* el siguiente parte de aqui */
    }
    return r;
}

static void mensaje(Resultado r, char *buf, size_t tam) {
    if (r.correcto)
        snprintf(buf, tam, "movimiento CORRECTO.");
    else if (r.letra)
        snprintf(buf, tam, "INCORRECTO falla letra %c no pertenece a un caballo.", r.letra);
    else
        snprintf(buf, tam, "INCORRECTO falla en el movimiento %d.", r.movimiento);
}

static int contar_movimientos(const char *s) {
    int n = 0, dentro = 0;
    for (; *s; s++) {
        if (isspace((unsigned char)*s)) dentro = 0;
        else if (!dentro) { dentro = 1; n++; }
    }
    return n;
}

/* Reloj de alta resolucion, en milisegundos. */
static double ahora_ms(void) {
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

/* ------------------------------------------- validacion de la entrada
 * Se valida el FORMATO de lo que escribe el usuario. Que un movimiento sea
 * correcto o no (por ejemplo "Qb4") lo decide verificar(), no estas funciones. */

enum { OK = 0, VACIO = 1, INVALIDO = 2, MENOR_QUE_1 = 3, MUY_GRANDE = 4 };

/* Quita espacios, pasa la columna a minuscula ("A1 " -> "a1") y valida.
 * salida debe tener al menos 3 bytes. */
static int validar_casilla(const char *texto, char *salida) {
    size_t n;
    int c, f;
    while (isspace((unsigned char)*texto)) texto++;
    n = strlen(texto);
    while (n > 0 && isspace((unsigned char)texto[n - 1])) n--;
    if (n == 0) return VACIO;
    if (n != 2) return INVALIDO;
    salida[0] = (char)tolower((unsigned char)texto[0]);
    salida[1] = texto[1];
    salida[2] = '\0';
    return parsear_casilla(salida, &c, &f) ? OK : INVALIDO;
}

/* n debe ser un entero positivo (solo digitos). */
static int validar_n(const char *texto, long *n) {
    size_t i, largo;
    while (isspace((unsigned char)*texto)) texto++;
    largo = strlen(texto);
    while (largo > 0 && isspace((unsigned char)texto[largo - 1])) largo--;
    if (largo == 0) return VACIO;
    for (i = 0; i < largo; i++)
        if (!isdigit((unsigned char)texto[i])) return INVALIDO;
    if (largo > 9) return MUY_GRANDE;
    *n = strtol(texto, NULL, 10);
    return *n < 1 ? MENOR_QUE_1 : OK;
}

static void explicar_casilla(int v, const char *texto) {
    if (v == VACIO) printf("  La posicion no puede estar vacia.\n");
    else printf("  '%s' no es una casilla valida: use una columna a-h seguida de una fila 1-8 (ejemplo: e4).\n", texto);
}

static void explicar_n(int v, const char *texto) {
    if (v == VACIO) printf("  n no puede estar vacio.\n");
    else if (v == MENOR_QUE_1) printf("  n debe ser al menos 1.\n");
    else if (v == MUY_GRANDE) printf("  n es demasiado grande.\n");
    else printf("  '%s' no es un entero positivo.\n", texto);
}

/* Lee una linea. Devuelve 1 si ok, 0 si se cerro la entrada, -1 si era demasiado larga. */
static int leer_linea(char *buf, size_t tam) {
    size_t n;
    int c;
    if (!fgets(buf, (int)tam, stdin)) return 0;
    n = strlen(buf);
    if (n > 0 && buf[n - 1] == '\n') { buf[n - 1] = '\0'; return 1; }
    if (feof(stdin)) return 1;                       /* ultima linea sin salto */
    while ((c = getchar()) != '\n' && c != EOF) {}   /* descartar el resto */
    return -1;
}

/* Pide Ak, n y C. Devuelve 1 si hay datos validos, 0 si se cerro la entrada. */
static int pedir_datos(char *inicio, long *n, char *cadena, size_t tam) {
    char linea[256];
    int r, v;

    for (;;) {                                        /* posicion inicial */
        printf("Posicion inicial del caballo (ej. a1): ");
        r = leer_linea(linea, sizeof linea);
        if (r == 0) return 0;
        if (r < 0) { printf("  Entrada demasiado larga.\n"); continue; }
        v = validar_casilla(linea, inicio);
        if (v == OK) break;
        explicar_casilla(v, linea);
    }
    for (;;) {                                        /* n y cadena */
        int cuenta;
        for (;;) {
            printf("Numero de movimientos n: ");
            r = leer_linea(linea, sizeof linea);
            if (r == 0) return 0;
            if (r < 0) { printf("  Entrada demasiado larga.\n"); continue; }
            v = validar_n(linea, n);
            if (v == OK) break;
            explicar_n(v, linea);
        }
        for (;;) {
            printf("Cadena de movimientos (ej. Kc2 Kd4 Kb3): ");
            r = leer_linea(cadena, tam);
            if (r == 0) return 0;
            if (r < 0) { printf("  La cadena es demasiado larga (maximo %d caracteres).\n", MAX_LINEA - 1); continue; }
            if (contar_movimientos(cadena) > 0) break;
            printf("  Debe ingresar al menos un movimiento.\n");
        }
        cuenta = contar_movimientos(cadena);
        if (cuenta == *n) return 1;
        printf("  Indico n = %ld, pero la cadena tiene %d movimientos. Ingrese n y la cadena de nuevo.\n",
               *n, cuenta);
    }
}

/* ------------------------------------------------------------ medicion */

/* Repite la verificacion REPETICIONES veces, mide y escribe el .txt.
 * Solo se mide la verificacion; no el tiempo que el usuario tarda en escribir. */
static void medir_y_guardar(const char *inicio, long n, const char *cadena, Resultado res) {
    double tiempos[REPETICIONES], total = 0.0;
    volatile int sumidero = 0;       /* evita que el compilador elimine el trabajo */
    char buf[128];
    FILE *out;
    int i;

    for (i = 0; i < REPETICIONES; i++) {
        double t0 = ahora_ms();
        Resultado r = verificar(inicio, cadena);
        double t1 = ahora_ms();
        sumidero += r.correcto;
        tiempos[i] = t1 - t0;
        total += tiempos[i];
    }
    mensaje(res, buf, sizeof buf);

    out = fopen(ARCHIVO_RESULTADOS, "w");
    if (!out) { printf("No se pudo crear %s\n", ARCHIVO_RESULTADOS); return; }
    fprintf(out, "Ejercicio 1 - Verificador de movimientos de caballo\n");
    fprintf(out, "Lenguaje: C\n");
    fprintf(out, "Casilla inicial: %s\n", inicio);
    fprintf(out, "Numero de movimientos n: %ld\n", n);
    fprintf(out, "Resultado de la verificacion: %s\n", buf);
    fprintf(out, "Repeticiones: %d\n", REPETICIONES);
#ifdef _WIN32
    fprintf(out, "Reloj: QueryPerformanceCounter\n\n");
#else
    fprintf(out, "Reloj: clock_gettime(CLOCK_MONOTONIC)\n\n");
#endif
    for (i = 0; i < REPETICIONES; i++)
        fprintf(out, "Repeticion %2d: %.6f ms\n", i + 1, tiempos[i]);
    fprintf(out, "\nTiempo total (%d repeticiones): %.6f ms\n", REPETICIONES, total);
    fprintf(out, "Tiempo promedio por repeticion: %.6f ms\n", total / REPETICIONES);
    fclose(out);

    printf("Tiempo total (%d repeticiones): %.6f ms (promedio %.6f ms)\n",
           REPETICIONES, total, total / REPETICIONES);
    printf("Resultados escritos en %s\n", ARCHIVO_RESULTADOS);
}

static void ejecutar(const char *inicio, long n, const char *cadena) {
    char buf[128];
    Resultado r = verificar(inicio, cadena);
    mensaje(r, buf, sizeof buf);
    printf("\nAk = %s, n = %ld: %s\n\n", inicio, n, buf);
    medir_y_guardar(inicio, n, cadena, r);
}

/* ---------------------------------------------------------------- pruebas */

typedef struct { const char *inicio, *cadena, *esperado; } Caso;

static const Caso CASOS[] = {
    {"a1", "Kc2 Kd4 Kb3 Kc5", "movimiento CORRECTO."},
    {"a1", "Kc2 Kd4 Kb4 Kc5", "INCORRECTO falla en el movimiento 3."},
    {"a2", "Kc3 Ke4 Kf6 Ke8 Kg7", "movimiento CORRECTO."},
    {"a2", "Kc8 Ke4 Kf6 Ke8 Kg7", "INCORRECTO falla en el movimiento 1."},
    {"a8", "Kb6 Kd7", "movimiento CORRECTO."},
    {"a8", "Kb6 Kb7", "INCORRECTO falla en el movimiento 2."},
    {"a1", "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Kb4", "movimiento CORRECTO."},
    {"a1", "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kc6 Qb4",
     "INCORRECTO falla letra Q no pertenece a un caballo."},
    {"a1", "Kc2 Kd4 Kf3 Ke5 Kg6 Kh8 Kf7 Kd8 Kh6 Kb4",
     "INCORRECTO falla en el movimiento 9."}
};
#define NUM_CASOS ((int)(sizeof(CASOS) / sizeof(CASOS[0])))

static const char *INICIOS_INVALIDOS[] = {
    "", "   ", "i1", "a9", "a0", "aa", "11", "a", "a11", "e 4"
};
static const char *N_INVALIDOS[] = {
    "", "  ", "0", "-3", "+5", "4.5", "abc", "1e3", "5 5", "99999999999"
};
#define NUM_INICIOS ((int)(sizeof(INICIOS_INVALIDOS) / sizeof(INICIOS_INVALIDOS[0])))
#define NUM_NS ((int)(sizeof(N_INVALIDOS) / sizeof(N_INVALIDOS[0])))

static void correr_pruebas(void) {
    int i, fallos = 0, extra = 0, total;
    char buf[128], casilla[3];
    long n;
    static const char *N_VALIDOS_TXT[] = {"4", " 10 ", "500"};
    static const long N_VALIDOS_VAL[] = {4, 10, 500};

    for (i = 0; i < NUM_CASOS; i++) {
        mensaje(verificar(CASOS[i].inicio, CASOS[i].cadena), buf, sizeof buf);
        if (strcmp(buf, CASOS[i].esperado) != 0) fallos++;
        printf("[%s] %s %s\n        -> %s\n",
               strcmp(buf, CASOS[i].esperado) == 0 ? "OK   " : "ERROR",
               CASOS[i].inicio, CASOS[i].cadena, buf);
    }
    for (i = 0; i < NUM_INICIOS; i++) {
        extra++;
        if (validar_casilla(INICIOS_INVALIDOS[i], casilla) == OK) {
            fallos++;
            printf("[ERROR] se acepto la posicion invalida '%s'\n", INICIOS_INVALIDOS[i]);
        }
    }
    extra++;
    if (validar_casilla(" E4 ", casilla) != OK || strcmp(casilla, "e4") != 0) {
        fallos++;
        printf("[ERROR] no se normalizo ' E4 '\n");
    }
    for (i = 0; i < 3; i++) {
        extra++;
        if (validar_n(N_VALIDOS_TXT[i], &n) != OK || n != N_VALIDOS_VAL[i]) {
            fallos++;
            printf("[ERROR] n valido rechazado: '%s'\n", N_VALIDOS_TXT[i]);
        }
    }
    for (i = 0; i < NUM_NS; i++) {
        extra++;
        if (validar_n(N_INVALIDOS[i], &n) == OK) {
            fallos++;
            printf("[ERROR] se acepto un n invalido: '%s'\n", N_INVALIDOS[i]);
        }
    }
    total = NUM_CASOS + extra;
    printf("\nPruebas: %d/%d correctas (%d ejemplos del enunciado + %d validaciones de entrada)\n",
           total - fallos, total, NUM_CASOS, extra);
}

/* ------------------------------------------------------------------ main */

int main(int argc, char **argv) {
    char inicio[3];
    long n;

    if (argc == 1) {
        char *cadena = (char *)malloc(MAX_LINEA);       /* memoria dinamica */
        if (!cadena) { printf("Sin memoria.\n"); return 1; }
        if (pedir_datos(inicio, &n, cadena, MAX_LINEA)) ejecutar(inicio, n, cadena);
        free(cadena);
    } else if (argc == 2 && strcmp(argv[1], "--pruebas") == 0) {
        correr_pruebas();
    } else if (argc == 4) {
        int v1 = validar_casilla(argv[1], inicio), v2 = validar_n(argv[2], &n);
        int cuenta = contar_movimientos(argv[3]);
        if (v1 != OK) explicar_casilla(v1, argv[1]);
        else if (v2 != OK) explicar_n(v2, argv[2]);
        else if (cuenta == 0) printf("Debe ingresar al menos un movimiento.\n");
        else if (cuenta != n) printf("Indico n = %ld, pero la cadena tiene %d movimientos.\n", n, cuenta);
        else ejecutar(inicio, n, argv[3]);
    } else {
        printf("Uso: caballo [inicio n \"cadena\"]  |  caballo --pruebas\n");
    }
    return 0;
}
