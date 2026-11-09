/* Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (C)
 *
 * El usuario escribe un entero no negativo n. El programa lo valida, genera el
 * triangulo de Pascal en memoria dinamica, muestra los coeficientes y el
 * polinomio (x+1)^n, y luego el calculo paso a paso de f(x) = (x+1)^n para un
 * valor fijo de x (X_EVALUACION), que NO se pide al usuario.
 * Si n = 100 se mide ademas el tiempo de generacion (resultados_c.txt).
 *
 * Los coeficientes son enteros de 128 bits (__int128, requiere gcc de 64 bits):
 * alcanza hasta n = 130. Python no tiene este limite.
 *
 * Uso:
 *   pascal              -> modo interactivo (pide n)
 *   pascal --pruebas    -> casos de prueba automaticos
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

typedef __int128 i128;

#define N_MAX 130            /* mayor n cuyos coeficientes caben en un i128 con signo */
#define X_EVALUACION 2       /* x fijo del calculo paso a paso (parte b) */
#define MOSTRAR_EVALUACION 1 /* 0 = no mostrar la parte b */
#define REPETICIONES 1000
#define ARCHIVO_RESULTADOS "resultados_c.txt"

/* ---------------------------------------------------------- utilidades */

/* printf no sabe imprimir __int128: se convierte a texto a mano (buf >= 48). */
static void i128_a_texto(i128 v, char *buf) {
    char tmp[48];
    int i = 0, j = 0;
    unsigned __int128 u = (v < 0) ? (unsigned __int128)(-(v + 1)) + 1
                                  : (unsigned __int128)v;
    if (u == 0) tmp[i++] = '0';
    while (u > 0) { tmp[i++] = (char)('0' + (int)(u % 10)); u /= 10; }
    if (v < 0) buf[j++] = '-';
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

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

/* ------------------------------------------------- triangulo de Pascal */

static void liberar_triangulo(i128 **t, int filas) {
    int i;
    if (!t) return;
    for (i = 0; i < filas; i++) free(t[i]);
    free(t);
}

/* Filas 0..n en memoria dinamica. La fila i tiene i+1 elementos y se calcula
 * a partir de la anterior (programacion dinamica). NULL si no hay memoria. */
static i128 **generar_triangulo(int n) {
    int i, k;
    i128 **t = (i128 **)malloc((size_t)(n + 1) * sizeof *t);
    if (!t) return NULL;
    for (i = 0; i <= n; i++) {
        t[i] = (i128 *)malloc((size_t)(i + 1) * sizeof **t);
        if (!t[i]) { liberar_triangulo(t, i); return NULL; }
        t[i][0] = t[i][i] = 1;
        for (k = 1; k < i; k++)
            t[i][k] = t[i - 1][k - 1] + t[i - 1][k];
    }
    return t;
}

/* fila[k] = C(n,k) es el coeficiente de x^(n-k). Ej.: {1,2,1} -> "x^2+2x+1".
 * El texto queda en memoria dinamica: quien llama debe hacer free(). */
static char *polinomio_texto(const i128 *fila, int n) {
    size_t cap = (size_t)(n + 1) * 56 + 8, pos = 0;
    char *s = (char *)malloc(cap), num[48];
    int k;
    if (!s) return NULL;
    for (k = 0; k <= n; k++) {
        int p = n - k;
        if (k > 0) s[pos++] = '+';
        if (p == 0) {
            i128_a_texto(fila[k], num);
            pos += (size_t)sprintf(s + pos, "%s", num);
        } else {
            if (fila[k] != 1) {
                i128_a_texto(fila[k], num);
                pos += (size_t)sprintf(s + pos, "%s", num);
            }
            s[pos++] = 'x';
            if (p > 1) pos += (size_t)sprintf(s + pos, "^%d", p);
        }
    }
    s[pos] = '\0';
    return s;
}

/* ------------------------------------------- parte (b): calculo paso a paso */

/* f(x) = suma de C(n,k) * x^(n-k). Devuelve 1 si el resultado es exacto, 0 si
 * algun valor no cabe en 128 bits (con mostrar=1 se avisa y se da un valor
 * aproximado en double). */
static int evaluar_paso_a_paso(const i128 *fila, int n, long long x,
                               int mostrar, i128 *resultado) {
    i128 *pot = (i128 *)malloc((size_t)(n + 1) * sizeof *pot);
    i128 acum = 0;
    char a[48], b[48], c[48], d[48];
    int k, p;
    if (!pot) return 0;

    pot[0] = 1;                                  /* pot[p] = x^p, tambien por DP */
    for (p = 1; p <= n; p++)
        if (__builtin_mul_overflow(pot[p - 1], (i128)x, &pot[p])) goto desborde;
    for (k = 0; k <= n; k++) {
        i128 termino;
        p = n - k;
        if (__builtin_mul_overflow(fila[k], pot[p], &termino) ||
            __builtin_add_overflow(acum, termino, &acum)) goto desborde;
        if (mostrar) {
            i128_a_texto(fila[k], a);
            i128_a_texto(pot[p], b);
            i128_a_texto(termino, c);
            i128_a_texto(acum, d);
            printf("Paso %3d: %s * (%lld)^%d = %s * %s = %s   acumulado = %s\n",
                   k + 1, a, x, p, a, b, c, d);
        }
    }
    free(pot);
    *resultado = acum;
    return 1;

desborde:
    free(pot);
    if (mostrar)
        printf("\nDesbordamiento: un valor no cabe en 128 bits (limite de C).\n"
               "Valor aproximado de (x+1)^n con double: %.6e\n",
               pow((double)x + 1.0, (double)n));
    return 0;
}

static void mostrar_evaluacion(const i128 *fila, int n) {
    i128 resultado, directo = 1;
    char buf[48], buf2[48];
    int k, directo_ok = 1;

    printf("Calculo paso a paso de f(x) = (x+1)^%d para x = %d:\n", n, X_EVALUACION);
    if (!evaluar_paso_a_paso(fila, n, X_EVALUACION, 1, &resultado)) return;
    for (k = 0; k < n && directo_ok; k++)
        directo_ok = !__builtin_mul_overflow(directo, (i128)(X_EVALUACION + 1), &directo);
    i128_a_texto(resultado, buf);
    printf("\nf(%d) = %s\n", X_EVALUACION, buf);
    if (directo_ok) {
        i128_a_texto(directo, buf2);
        printf("Verificacion directa (x+1)^n = %s -> %s\n", buf2,
               directo == resultado ? "coincide" : "NO COINCIDE");
    }
}

/* ------------------------------------------- validacion y lectura interactiva */

enum { OK = 0, VACIO = 1, INVALIDO = 2, NEGATIVO = 3, MUY_GRANDE = 4 };

/* n debe ser un entero no negativo escrito solo con digitos (se ignoran los
 * espacios de los extremos). Devuelve OK o el motivo del rechazo. */
static int validar_n(const char *texto, long *n) {
    size_t i, ini = 0, fin = strlen(texto);
    while (ini < fin && isspace((unsigned char)texto[ini])) ini++;
    while (fin > ini && isspace((unsigned char)texto[fin - 1])) fin--;
    if (fin == ini) return VACIO;
    if (texto[ini] == '-') return NEGATIVO;
    for (i = ini; i < fin; i++)
        if (!isdigit((unsigned char)texto[i])) return INVALIDO;
    if (fin - ini > 9) return MUY_GRANDE;
    *n = strtol(texto + ini, NULL, 10);
    return *n > N_MAX ? MUY_GRANDE : OK;
}

static void explicar_n(int v, const char *texto) {
    if (v == VACIO) printf("  El campo no puede estar vacio.\n");
    else if (v == NEGATIVO) printf("  Debe ser un numero entero no negativo (sin signo negativo).\n");
    else if (v == MUY_GRANDE) printf("  n supera el maximo (%d) que cabe en enteros de 128 bits.\n", N_MAX);
    else printf("  '%s' no es un numero entero valido (no use letras ni simbolos).\n", texto);
}

/* Lee una linea. Devuelve 1 si ok, 0 si se cerro la entrada, -1 si era demasiado larga. */
static int leer_linea(char *buf, size_t tam) {
    size_t n;
    int c;
    if (!fgets(buf, (int)tam, stdin)) return 0;
    n = strlen(buf);
    if (n > 0 && buf[n - 1] == '\n') { buf[n - 1] = '\0'; return 1; }
    if (feof(stdin)) return 1;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return -1;
}

/* Pide n hasta que sea valido. Devuelve 0 si se cierra la entrada. */
static int pedir_n(long *n) {
    char linea[256];
    for (;;) {
        int r, v;
        printf("Ingrese un numero entero no negativo n: ");
        r = leer_linea(linea, sizeof linea);
        if (r == 0) return 0;
        if (r < 0) { printf("  Entrada demasiado larga.\n"); continue; }
        v = validar_n(linea, n);
        if (v == OK) return 1;
        explicar_n(v, linea);
    }
}

/* ------------------------------------------------------------ medicion (n = 100) */

static void medir_n100(void) {
    volatile i128 sumidero = 0;      /* evita que el compilador elimine el trabajo */
    i128 **t;
    double t0, t1, total;
    char central[48], *texto;
    FILE *out;
    int r;

    printf("\n[Ejecutando prueba de rendimiento para n = 100...]\n");
    t0 = ahora_ms();
    for (r = 0; r < REPETICIONES; r++) {
        t = generar_triangulo(100);
        if (!t) { printf("Sin memoria.\n"); return; }
        sumidero = t[100][50];
        liberar_triangulo(t, 101);
    }
    t1 = ahora_ms();
    total = t1 - t0;

    t = generar_triangulo(100);
    texto = t ? polinomio_texto(t[100], 100) : NULL;
    i128_a_texto(sumidero, central);

    out = fopen(ARCHIVO_RESULTADOS, "w");
    if (out && texto) {
        fprintf(out, "Ejercicio 2 - Triangulo de Pascal y polinomio (x+1)^n (n = 100)\n");
        fprintf(out, "Lenguaje: C (__int128)\n");
        fprintf(out, "Se generan los polinomios de n = 0 hasta n = 100 (101 filas)\n");
        fprintf(out, "Repeticiones: %d\n", REPETICIONES);
#ifdef _WIN32
        fprintf(out, "Reloj: QueryPerformanceCounter\n\n");
#else
        fprintf(out, "Reloj: clock_gettime(CLOCK_MONOTONIC)\n\n");
#endif
        fprintf(out, "Tiempo total (%d repeticiones): %.6f ms\n", REPETICIONES, total);
        fprintf(out, "Tiempo promedio por generacion completa: %.6f ms\n", total / REPETICIONES);
        fprintf(out, "Coeficiente central C(100,50) = %s\n\n", central);
        fprintf(out, "Polinomio (x+1)^100:\n%s\n", texto);
        fclose(out);
        printf("Resultados guardados en %s\n", ARCHIVO_RESULTADOS);
    } else {
        printf("No se pudo escribir %s\n", ARCHIVO_RESULTADOS);
        if (out) fclose(out);
    }
    printf("Tiempo total (%d repeticiones para n=100): %.6f ms (promedio: %.6f ms)\n",
           REPETICIONES, total, total / REPETICIONES);
    free(texto);
    liberar_triangulo(t, 101);
}

/* ---------------------------------------------------------------- pruebas */

static const char *ESPERADOS[] = {
    "1", "x+1", "x^2+2x+1", "x^3+3x^2+3x+1", "x^4+4x^3+6x^2+4x+1",
    "x^5+5x^4+10x^3+10x^2+5x+1", "x^6+6x^5+15x^4+20x^3+15x^2+6x+1",
    "x^7+7x^6+21x^5+35x^4+35x^3+21x^2+7x+1"
};
static const char *N_VALIDOS[] = {"0", "5", " 12 ", "100", "130", "007"};
static const long N_VALIDOS_VAL[] = {0, 5, 12, 100, 130, 7};
static const char *N_INVALIDOS[] = {
    "", "   ", "abc", "-5", "-", "+", "+5", "4.5", "1e3", "5 5", "5a", "131",
    "99999999999"
};
#define NUM_VALIDOS ((int)(sizeof(N_VALIDOS) / sizeof(N_VALIDOS[0])))
#define NUM_INVALIDOS ((int)(sizeof(N_INVALIDOS) / sizeof(N_INVALIDOS[0])))

static int correr_pruebas(void) {
    int n, k, i, errores = 0;
    long v;
    static const int NS[] = {0, 1, 5, 10, 30};
    static const long long XS[] = {-3, -1, 0, 1, 2, 7};
    i128 **t = generar_triangulo(N_MAX);
    if (!t) return 0;

    for (n = 0; n < 8; n++) {                            /* ejemplos del enunciado */
        char *s = polinomio_texto(t[n], n);
        int ok = s && strcmp(s, ESPERADOS[n]) == 0;
        printf("[%s] (x+1)^%d = %s\n", ok ? "OK   " : "ERROR", n, s);
        errores += !ok;
        free(s);
    }
    for (n = 0; n <= 100; n++) {                         /* simetria y suma = 2^n */
        i128 suma = 0;
        for (k = 0; k <= n; k++) {
            suma += t[n][k];
            if (t[n][k] != t[n][n - k]) errores++;
        }
        if (suma != ((i128)1 << n)) errores++;
    }
    for (i = 0; i < 5; i++) {                            /* evaluacion vs potencia directa */
        int j;
        for (j = 0; j < 6; j++) {
            i128 res, directo = 1;
            int m;
            for (m = 0; m < NS[i]; m++) directo *= (i128)XS[j] + 1;
            if (!evaluar_paso_a_paso(t[NS[i]], NS[i], XS[j], 0, &res) || res != directo)
                errores++;
        }
    }
    for (i = 0; i < NUM_VALIDOS; i++)                    /* validacion de n */
        if (validar_n(N_VALIDOS[i], &v) != OK || v != N_VALIDOS_VAL[i]) {
            errores++;
            printf("[ERROR] n valido rechazado: '%s'\n", N_VALIDOS[i]);
        }
    for (i = 0; i < NUM_INVALIDOS; i++)
        if (validar_n(N_INVALIDOS[i], &v) == OK) {
            errores++;
            printf("[ERROR] se acepto un n invalido: '%s'\n", N_INVALIDOS[i]);
        }
    printf("\nPruebas: %s\n\n", errores == 0 ? "todas correctas" : "HAY ERRORES");
    liberar_triangulo(t, N_MAX + 1);
    return errores == 0;
}

int main(int argc, char **argv) {
    long n;
    i128 **t;
    char buf[48], *texto;
    int k;

    if (argc == 2 && strcmp(argv[1], "--pruebas") == 0) {
        correr_pruebas();
        return 0;
    }
    if (argc != 1) {
        printf("Uso: pascal  |  pascal --pruebas\n");
        return 0;
    }
    if (!pedir_n(&n)) return 0;

    t = generar_triangulo((int)n);
    if (!t) { printf("Sin memoria.\n"); return 1; }
    printf("\nCoeficientes de (x+1)^%ld:", n);
    for (k = 0; k <= n; k++) {
        i128_a_texto(t[n][k], buf);
        printf(" %s", buf);
    }
    texto = polinomio_texto(t[n], (int)n);
    printf("\n\n(x+1)^%ld = %s\n\n", n, texto ? texto : "?");
    free(texto);
    if (MOSTRAR_EVALUACION) mostrar_evaluacion(t[n], (int)n);
    liberar_triangulo(t, (int)n + 1);

    if (n == 100) medir_n100();      /* la catedra pide el tiempo para n = 100 */
    return 0;
}
