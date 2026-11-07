/* Ejercicio 3 - Palabras reservadas de C: deteccion, traduccion y resaltado (C)
 *
 * Carga un archivo de codigo C en memoria dinamica, recorre TODO el codigo y
 * reemplaza solo las palabras reservadas por su equivalente en espanol,
 * resaltandolas. El resto del codigo (nombres, comentarios, textos, directivas)
 * queda intacto.
 *
 * Uso:
 *   traductor                 -> pide la ruta del archivo C
 *   traductor programa.c      -> usa ese archivo
 *   traductor --pruebas       -> casos de prueba del traductor
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <time.h>
#include <unistd.h>
#endif

#define REPETICIONES 100
#define ARCHIVO_EJEMPLO "ejemplo.c"
#define ARCHIVO_CODIGO "codigo_traducido.txt"
#define ARCHIVO_RESULTADOS "resultados_c.txt"
#define MARCA "`"                        /* resaltado en texto plano: `entero` */
#define ANSI_ABRIR "\x1b[30;43m"         /* texto negro sobre fondo amarillo */
#define ANSI_CERRAR "\x1b[0m"

/* Palabras reservadas de C (C89 + C99 + C11 = 44) y su equivalente en espanol.
 * Sin acentos ni enie para que se vean bien en la consola de Windows. */
typedef struct { const char *c, *es; } Palabra;

static const Palabra TABLA[] = {
    {"auto", "automatico"}, {"break", "romper"}, {"case", "caso"},
    {"char", "caracter"}, {"const", "constante"}, {"continue", "continuar"},
    {"default", "predeterminado"}, {"do", "hacer"}, {"double", "doble"},
    {"else", "sino"}, {"enum", "enumeracion"}, {"extern", "externo"},
    {"float", "flotante"}, {"for", "para"}, {"goto", "ir_a"}, {"if", "si"},
    {"int", "entero"}, {"long", "largo"}, {"register", "registro"},
    {"return", "retornar"}, {"short", "corto"}, {"signed", "con_signo"},
    {"sizeof", "tamano_de"}, {"static", "estatico"}, {"struct", "estructura"},
    {"switch", "segun"}, {"typedef", "definir_tipo"}, {"union", "union"},
    {"unsigned", "sin_signo"}, {"void", "vacio"}, {"volatile", "volatil"},
    {"while", "mientras"},
    /* C99 */
    {"inline", "en_linea"}, {"restrict", "restringido"}, {"_Bool", "booleano"},
    {"_Complex", "complejo"}, {"_Imaginary", "imaginario"},
    /* C11 */
    {"_Alignas", "alinear_como"}, {"_Alignof", "alineacion_de"},
    {"_Atomic", "atomico"}, {"_Generic", "generico"}, {"_Noreturn", "sin_retorno"},
    {"_Static_assert", "asercion_estatica"}, {"_Thread_local", "local_al_hilo"}
};
#define NUM_PALABRAS ((int)(sizeof(TABLA) / sizeof(TABLA[0])))
#define LARGO_MAX_PALABRA 14             /* "_Static_assert" */

/* ------------------------------------------- buffer de salida dinamico */

typedef struct { char *datos; size_t len, cap; int fallo; } Buffer;

static void buf_init(Buffer *b) {
    b->len = 0; b->cap = 256; b->fallo = 0;
    b->datos = (char *)malloc(b->cap);
    if (!b->datos) b->fallo = 1;
}

/* Agrega n bytes; si falta espacio, duplica la capacidad con realloc. */
static void buf_add(Buffer *b, const char *s, size_t n) {
    if (b->fallo) return;
    if (b->len + n > b->cap) {
        size_t nueva = b->cap;
        char *p;
        while (nueva < b->len + n) nueva *= 2;
        p = (char *)realloc(b->datos, nueva);
        if (!p) { b->fallo = 1; return; }
        b->datos = p;
        b->cap = nueva;
    }
    memcpy(b->datos + b->len, s, n);
    b->len += n;
}

static void buf_liberar(Buffer *b) { free(b->datos); b->datos = NULL; b->len = b->cap = 0; }

/* ----------------------------------------------------------- traductor */

static int es_inicio_id(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
static int es_parte_id(char c) { return es_inicio_id(c) || (c >= '0' && c <= '9'); }

/* Indice de la palabra en TABLA, o -1 si no es palabra reservada. */
static int buscar(const char *s, size_t largo) {
    char tmp[LARGO_MAX_PALABRA + 1];
    int i;
    if (largo > LARGO_MAX_PALABRA) return -1;
    memcpy(tmp, s, largo);
    tmp[largo] = '\0';
    for (i = 0; i < NUM_PALABRAS; i++)
        if (strcmp(tmp, TABLA[i].c) == 0) return i;
    return -1;
}

/* Recorre s[0..n) y escribe en out el codigo con las palabras reservadas
 * traducidas y rodeadas por abrir/cerrar. conteo[i] cuenta las apariciones de
 * TABLA[i]. Solo se traducen palabras reservadas que sean codigo real: se
 * respetan comentarios, textos, caracteres, numeros, nombres que contienen una
 * palabra reservada (interior, entero_largo) y las directivas (#include <float.h>).
 * Devuelve 0 si se acabo la memoria. */
static int traducir(const char *s, size_t n, const char *abrir, const char *cerrar,
                    Buffer *out, int *conteo) {
    size_t i = 0, j, k, fin;
    int inicio_linea = 1;                 /* solo hubo espacios desde el ultimo salto */
    size_t la = strlen(abrir), lc = strlen(cerrar);

    memset(conteo, 0, (size_t)NUM_PALABRAS * sizeof(int));
    while (i < n) {
        char c = s[i];
        if (c == '\n') {
            buf_add(out, &c, 1); i++; inicio_linea = 1;
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v') {
            buf_add(out, &c, 1); i++;
        } else if (c == '#' && inicio_linea) {              /* directiva del preprocesador */
            j = i + 1;
            while (j < n && (s[j] == ' ' || s[j] == '\t')) j++;
            k = j;
            while (k < n && es_parte_id(s[k])) k++;
            if (k - j == 7 && memcmp(s + j, "include", 7) == 0)   /* <float.h> no es float */
                while (k < n && s[k] != '\n') k++;
            buf_add(out, s + i, k - i);                     /* el resto de la linea se procesa normal */
            i = k; inicio_linea = 0;
        } else {
            inicio_linea = 0;
            if (c == '/' && i + 1 < n && s[i + 1] == '/') {          /* comentario de linea */
                fin = i;
                while (fin < n && s[fin] != '\n') fin++;
                buf_add(out, s + i, fin - i); i = fin;
            } else if (c == '/' && i + 1 < n && s[i + 1] == '*') {   /* comentario de bloque */
                fin = i + 2;
                while (fin + 1 < n && !(s[fin] == '*' && s[fin + 1] == '/')) fin++;
                fin = (fin + 1 < n) ? fin + 2 : n;
                buf_add(out, s + i, fin - i); i = fin;
            } else if (c == '"' || c == '\'') {                      /* texto o caracter */
                j = i + 1;
                while (j < n && s[j] != c && s[j] != '\n') j += (s[j] == '\\') ? 2 : 1;
                if (j < n && s[j] == c) j++;
                else if (j > n) j = n;
                buf_add(out, s + i, j - i); i = j;
            } else if (c >= '0' && c <= '9') {                       /* numero (0x1f, 1e3...) */
                j = i + 1;
                while (j < n && (es_parte_id(s[j]) || s[j] == '.')) j++;
                buf_add(out, s + i, j - i); i = j;
            } else if (es_inicio_id(c)) {                            /* nombre o palabra reservada */
                int idx;
                j = i + 1;
                while (j < n && es_parte_id(s[j])) j++;
                idx = buscar(s + i, j - i);
                if (idx >= 0) {
                    buf_add(out, abrir, la);
                    buf_add(out, TABLA[idx].es, strlen(TABLA[idx].es));
                    buf_add(out, cerrar, lc);
                    conteo[idx]++;
                } else {
                    buf_add(out, s + i, j - i);
                }
                i = j;
            } else {
                buf_add(out, &c, 1); i++;
            }
        }
    }
    return !out->fallo;
}

static void totales(const int *conteo, int *total, int *distintas) {
    int i;
    *total = *distintas = 0;
    for (i = 0; i < NUM_PALABRAS; i++) {
        *total += conteo[i];
        if (conteo[i] > 0) (*distintas)++;
    }
}

/* ------------------------------------------------------ entrada / salida */

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

/* 1 si la salida admite colores ANSI (no si se redirige a un archivo). */
static int activar_color(void) {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    if (h == INVALID_HANDLE_VALUE || !GetConsoleMode(h, &modo)) return 0;
    return SetConsoleMode(h, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
    return isatty(1);
#endif
}

enum { CARGA_OK = 0, CARGA_NO_ABRE = 1, CARGA_VACIO = 2, CARGA_SIN_MEMORIA = 3 };

/* Carga el archivo completo en memoria dinamica (el llamador hace free). */
static char *cargar_archivo(const char *ruta, size_t *n, int *error) {
    FILE *f = fopen(ruta, "rb");
    long tam;
    char *datos;
    size_t i, leidos, vacio = 1;

    *error = CARGA_OK;
    if (!f) { *error = CARGA_NO_ABRE; return NULL; }
    fseek(f, 0, SEEK_END);
    tam = ftell(f);
    rewind(f);
    if (tam < 0) { fclose(f); *error = CARGA_NO_ABRE; return NULL; }
    datos = (char *)malloc((size_t)tam + 1);
    if (!datos) { fclose(f); *error = CARGA_SIN_MEMORIA; return NULL; }
    leidos = fread(datos, 1, (size_t)tam, f);
    fclose(f);
    datos[leidos] = '\0';
    for (i = 0; i < leidos; i++)
        if (!isspace((unsigned char)datos[i])) { vacio = 0; break; }
    if (vacio) { free(datos); *error = CARGA_VACIO; return NULL; }
    *n = leidos;
    return datos;
}

static void explicar_error(int error, const char *ruta) {
    if (error == CARGA_NO_ABRE) printf("  No se pudo abrir '%s' (no existe o no hay permiso).\n", ruta);
    else if (error == CARGA_VACIO) printf("  El archivo '%s' esta vacio.\n", ruta);
    else printf("  No hay memoria suficiente para cargar '%s'.\n", ruta);
}

/* Quita espacios y comillas de los extremos (al arrastrar un archivo a la consola). */
static void limpiar_ruta(char *s) {
    size_t n = strlen(s), ini = 0;
    while (n > 0 && (isspace((unsigned char)s[n - 1]) || s[n - 1] == '"')) s[--n] = '\0';
    while (s[ini] && (isspace((unsigned char)s[ini]) || s[ini] == '"')) ini++;
    memmove(s, s + ini, strlen(s + ini) + 1);
}

/* Pide la ruta hasta que el archivo exista y tenga contenido. NULL si se cierra la entrada. */
static char *pedir_archivo(size_t *n, char *ruta, size_t tam) {
    for (;;) {
        int error;
        char *codigo;
        printf("Ruta del archivo C (Enter = %s): ", ARCHIVO_EJEMPLO);
        if (!fgets(ruta, (int)tam, stdin)) return NULL;
        if (!strchr(ruta, '\n') && !feof(stdin)) {            /* linea demasiado larga */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("  Ruta demasiado larga.\n");
            continue;
        }
        limpiar_ruta(ruta);
        if (ruta[0] == '\0') { strncpy(ruta, ARCHIVO_EJEMPLO, tam - 1); ruta[tam - 1] = '\0'; }
        codigo = cargar_archivo(ruta, n, &error);
        if (codigo) return codigo;
        explicar_error(error, ruta);
    }
}

static const char *nombre_base(const char *ruta) {
    const char *a = strrchr(ruta, '\\'), *b = strrchr(ruta, '/');
    const char *p = a > b ? a : b;
    return p ? p + 1 : ruta;
}

static void procesar(const char *ruta, const char *codigo, size_t n) {
    Buffer pantalla, plano, prueba;
    int conteo[NUM_PALABRAS], conteo_aux[NUM_PALABRAS];
    int total, distintas, color, r;
    double t0, t1, total_ms;
    volatile size_t sumidero = 0;       /* evita que el compilador elimine el trabajo */
    size_t lineas = 1, i;
    FILE *out;

    color = activar_color();
    buf_init(&pantalla);
    if (color) r = traducir(codigo, n, ANSI_ABRIR, ANSI_CERRAR, &pantalla, conteo);
    else r = traducir(codigo, n, MARCA, MARCA, &pantalla, conteo);
    if (!r) { printf("Sin memoria.\n"); buf_liberar(&pantalla); return; }

    printf("\n");
    fwrite(pantalla.datos, 1, pantalla.len, stdout);
    if (pantalla.len == 0 || pantalla.datos[pantalla.len - 1] != '\n') printf("\n");
    printf("\n");
    totales(conteo, &total, &distintas);
    if (total > 0)
        printf("Se encontraron %d palabras reservadas (%d distintas) y se tradujeron al espanol.\n",
               total, distintas);
    else
        printf("No se encontraron palabras reservadas de C en el codigo.\n");
    if (!color)
        printf("(Resaltadas entre %scomillas invertidas%s; ejecute desde cmd para ver colores.)\n",
               MARCA, MARCA);
    buf_liberar(&pantalla);

    buf_init(&plano);                                        /* version en texto plano */
    if (!traducir(codigo, n, MARCA, MARCA, &plano, conteo_aux)) { printf("Sin memoria.\n"); buf_liberar(&plano); return; }
    out = fopen(ARCHIVO_CODIGO, "wb");
    if (out) { fwrite(plano.datos, 1, plano.len, out); fclose(out); }
    else printf("No se pudo crear %s\n", ARCHIVO_CODIGO);
    buf_liberar(&plano);

    t0 = ahora_ms();                                         /* medicion: solo la traduccion */
    for (i = 0; i < REPETICIONES; i++) {
        buf_init(&prueba);
        traducir(codigo, n, MARCA, MARCA, &prueba, conteo_aux);
        sumidero += prueba.len;
        buf_liberar(&prueba);
    }
    t1 = ahora_ms();
    total_ms = t1 - t0;

    for (i = 0; i < n; i++) if (codigo[i] == '\n') lineas++;
    out = fopen(ARCHIVO_RESULTADOS, "w");
    if (out) {
        fprintf(out, "Ejercicio 3 - Palabras reservadas de C\n");
        fprintf(out, "Lenguaje: C\n");
        fprintf(out, "Archivo analizado: %s\n", nombre_base(ruta));
        fprintf(out, "Tamano: %lu bytes, %lu lineas\n", (unsigned long)n, (unsigned long)lineas);
        fprintf(out, "Palabras reservadas encontradas: %d (%d distintas)\n", total, distintas);
        fprintf(out, "Repeticiones: %d\n", REPETICIONES);
#ifdef _WIN32
        fprintf(out, "Reloj: QueryPerformanceCounter\n");
#else
        fprintf(out, "Reloj: clock_gettime(CLOCK_MONOTONIC)\n");
#endif
        fprintf(out, "Se mide solo la traduccion del codigo ya cargado en memoria.\n\n");
        fprintf(out, "Tiempo total (%d repeticiones): %.6f ms\n", REPETICIONES, total_ms);
        fprintf(out, "Tiempo promedio por repeticion: %.6f ms\n", total_ms / REPETICIONES);
        fclose(out);
    } else {
        printf("No se pudo crear %s\n", ARCHIVO_RESULTADOS);
    }
    printf("\nTiempo total (%d repeticiones): %.6f ms (promedio %.6f ms)\n",
           REPETICIONES, total_ms, total_ms / REPETICIONES);
    printf("Codigo traducido escrito en %s\n", ARCHIVO_CODIGO);
    printf("Resultados escritos en %s\n", ARCHIVO_RESULTADOS);
}

/* --------------------------------------------------------------- pruebas */

typedef struct { const char *entrada, *esperado; } Caso;

static const Caso CASOS[] = {
    {"int x = 5;", "entero x = 5;"},
    {"printf(\"if while int\");", "printf(\"if while int\");"},
    {"// int en comentario\nint a;", "// int en comentario\nentero a;"},
    {"/* for */ for(;;)", "/* for */ para(;;)"},
    {"integer interior format_if int_x", "integer interior format_if int_x"},
    {"#include <float.h>\nfloat f;", "#include <float.h>\nflotante f;"},
    {"#if X\n#else\n#endif", "#if X\n#else\n#endif"},
    {"#define T unsigned int", "#define T sin_signo entero"},
    {"char c = 'i'; char q = '\\'';", "caracter c = 'i'; caracter q = '\\'';"},
    {"s = \"a\\\"while\"; while (1)", "s = \"a\\\"while\"; mientras (1)"},
    {"return 0;", "retornar 0;"},
    {"x = 0x1f + 1e3;", "x = 0x1f + 1e3;"},
    {"_Bool b; sizeof(b);", "booleano b; tamano_de(b);"},
    {"  # include <stdio.h>\nif (x) else", "  # include <stdio.h>\nsi (x) sino"},
    {"/* sin cerrar int", "/* sin cerrar int"},
    {"", ""}
};
#define NUM_CASOS ((int)(sizeof(CASOS) / sizeof(CASOS[0])))

static int indice_de(const char *palabra) { return buscar(palabra, strlen(palabra)); }

/* Traduce entrada y compara con esperado. Devuelve 1 si coincide. */
static int caso_ok(const char *entrada, const char *abrir, const char *cerrar,
                   const char *esperado, int *conteo) {
    Buffer b;
    int ok;
    buf_init(&b);
    traducir(entrada, strlen(entrada), abrir, cerrar, &b, conteo);
    buf_add(&b, "", 1);                                     /* terminador */
    ok = !b.fallo && strcmp(b.datos, esperado) == 0;
    if (!ok && !b.fallo) printf("        esperado: \"%s\"\n        obtenido: \"%s\"\n", esperado, b.datos);
    buf_liberar(&b);
    return ok;
}

static void correr_pruebas(void) {
    int i, fallos = 0, total, conteo[NUM_PALABRAS];

    for (i = 0; i < NUM_CASOS; i++) {
        int ok = caso_ok(CASOS[i].entrada, "", "", CASOS[i].esperado, conteo);
        printf("[%s] \"%s\"\n", ok ? "OK   " : "ERROR", CASOS[i].entrada);
        if (!ok) fallos++;
    }
    caso_ok("int a; if (a) { int b; } return a;", "", "", "entero a; si (a) { entero b; } retornar a;", conteo);
    if (conteo[indice_de("int")] != 2 || conteo[indice_de("if")] != 1 ||
        conteo[indice_de("return")] != 1) {
        fallos++;
        printf("[ERROR] conteo incorrecto\n");
    }
    if (!caso_ok("int x;", "<", ">", "<entero> x;", conteo)) {
        fallos++;
        printf("[ERROR] resaltado incorrecto\n");
    }
    total = NUM_CASOS + 2;
    printf("\nPruebas: %d/%d correctas (%d palabras reservadas en la tabla)\n",
           total - fallos, total, NUM_PALABRAS);
}

int main(int argc, char **argv) {
    char ruta[1024];
    size_t n = 0;
    char *codigo;
    int error;

    if (argc == 1) {
        codigo = pedir_archivo(&n, ruta, sizeof ruta);
        if (codigo) { procesar(ruta, codigo, n); free(codigo); }
    } else if (argc == 2 && strcmp(argv[1], "--pruebas") == 0) {
        correr_pruebas();
    } else if (argc == 2) {
        strncpy(ruta, argv[1], sizeof ruta - 1);
        ruta[sizeof ruta - 1] = '\0';
        limpiar_ruta(ruta);
        codigo = cargar_archivo(ruta, &n, &error);
        if (codigo) { procesar(ruta, codigo, n); free(codigo); }
        else explicar_error(error, ruta);
    } else {
        printf("Uso: traductor [archivo.c]  |  traductor --pruebas\n");
    }
    return 0;
}
