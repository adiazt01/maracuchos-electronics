#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *ingles;
    const char *espanol;
} PalabraReservada;

static const PalabraReservada PALABRAS[] = {
    /* C89 / C90 */
    {"auto", "automatico"},
    {"break", "romper"},
    {"case", "caso"},
    {"char", "caracter"},
    {"const", "constante"},
    {"continue", "continuar"},
    {"default", "predeterminado"},
    {"do", "hacer"},
    {"double", "doble"},
    {"else", "sino"},
    {"enum", "enumeracion"},
    {"extern", "externo"},
    {"float", "flotante"},
    {"for", "para"},
    {"goto", "ir_a"},
    {"if", "si"},
    {"int", "entero"},
    {"long", "largo"},
    {"register", "registro"},
    {"return", "retornar"},
    {"short", "corto"},
    {"signed", "con_signo"},
    {"sizeof", "tamano_de"},
    {"static", "estatico"},
    {"struct", "estructura"},
    {"switch", "segun"},
    {"typedef", "definir_tipo"},
    {"union", "union"},
    {"unsigned", "sin_signo"},
    {"void", "vacio"},
    {"volatile", "volatil"},
    {"while", "mientras"},

    /* C99 */
    {"inline", "en_linea"},
    {"restrict", "restringido"},
    {"_Bool", "_Booleano"},
    {"_Complex", "_Complejo"},
    {"_Imaginary", "_Imaginario"},

    /* C11 */
    {"_Alignas", "_Alinear"},
    {"_Alignof", "_Alineacion_de"},
    {"_Atomic", "_Atomico"},
    {"_Generic", "_Generico"},
    {"_Noreturn", "_SinRetorno"},
    {"_Static_assert", "_Afirmacion_estatica"},
    {"_Thread_local", "_Hilo_local"},
};

#define NUM_PALABRAS (sizeof(PALABRAS) / sizeof(PALABRAS[0]))

typedef struct {
    char *datos;
    size_t longitud;
    size_t capacidad;
} Buffer;

static void buffer_iniciar(Buffer *buffer, size_t capacidad) {
    buffer->datos = malloc(capacidad);
    if (buffer->datos == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    buffer->longitud = 0;
    buffer->capacidad = capacidad;
    buffer->datos[0] = '\0';
}

static void buffer_reservar(Buffer *buffer, size_t extra) {
    size_t necesaria = buffer->longitud + extra + 1;
    if (necesaria <= buffer->capacidad) {
        return;
    }

    size_t nueva = buffer->capacidad;
    while (nueva < necesaria) {
        nueva *= 2;
    }

    char *temporal = realloc(buffer->datos, nueva);
    if (temporal == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    buffer->datos = temporal;
    buffer->capacidad = nueva;
}

static void buffer_agregar(Buffer *buffer, const char *texto, size_t longitud) {
    buffer_reservar(buffer, longitud);
    memcpy(buffer->datos + buffer->longitud, texto, longitud);
    buffer->longitud += longitud;
    buffer->datos[buffer->longitud] = '\0';
}

static void buffer_caracter(Buffer *buffer, char caracter) {
    buffer_agregar(buffer, &caracter, 1);
}

static void buffer_liberar(Buffer *buffer) {
    free(buffer->datos);
    buffer->datos = NULL;
    buffer->longitud = 0;
    buffer->capacidad = 0;
}

static char *leer_archivo(const char *ruta, size_t *longitud) {
    FILE *archivo = fopen(ruta, "rb");
    if (archivo == NULL) {
        perror(ruta);
        exit(EXIT_FAILURE);
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    long tamano = ftell(archivo);
    if (tamano < 0) {
        perror("ftell");
        fclose(archivo);
        exit(EXIT_FAILURE);
    }
    rewind(archivo);

    char *datos = malloc((size_t)tamano + 1);
    if (datos == NULL) {
        perror("malloc");
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    size_t leidos = fread(datos, 1, (size_t)tamano, archivo);
    if (leidos != (size_t)tamano) {
        fprintf(stderr, "Error al leer el archivo: %s\n", ruta);
        free(datos);
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    datos[tamano] = '\0';
    fclose(archivo);
    *longitud = (size_t)tamano;
    return datos;
}

static int es_inicio_identificador(unsigned char caracter) {
    return isalpha(caracter) || caracter == '_';
}

static int es_continuacion_identificador(unsigned char caracter) {
    return isalnum(caracter) || caracter == '_';
}

static int buscar_palabra(const char *texto, size_t longitud) {
    for (size_t i = 0; i < NUM_PALABRAS; i++) {
        if (strlen(PALABRAS[i].ingles) == longitud &&
            strncmp(texto, PALABRAS[i].ingles, longitud) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static void procesar(const char *fuente, size_t longitud, Buffer *salida,
                     unsigned long *conteos, unsigned long *total,
                     int traducir) {
    size_t i = 0;

    while (i < longitud) {
        char actual = fuente[i];

        if (actual == '/' && i + 1 < longitud && fuente[i + 1] == '/') {
            size_t inicio = i;
            while (i < longitud && fuente[i] != '\n') {
                i++;
            }
            if (i < longitud) {
                i++;
            }
            buffer_agregar(salida, fuente + inicio, i - inicio);
            continue;
        }

        if (actual == '/' && i + 1 < longitud && fuente[i + 1] == '*') {
            size_t inicio = i;
            i += 2;
            while (i + 1 < longitud &&
                   !(fuente[i] == '*' && fuente[i + 1] == '/')) {
                i++;
            }
            if (i + 1 < longitud) {
                i += 2;
            } else {
                i = longitud;
            }
            buffer_agregar(salida, fuente + inicio, i - inicio);
            continue;
        }

        if (actual == '"' || actual == '\'') {
            char comilla = actual;
            size_t inicio = i;
            i++;
            while (i < longitud) {
                if (fuente[i] == '\\' && i + 1 < longitud) {
                    i += 2;
                    continue;
                }
                if (fuente[i] == comilla) {
                    i++;
                    break;
                }
                i++;
            }
            buffer_agregar(salida, fuente + inicio, i - inicio);
            continue;
        }

        if (es_inicio_identificador((unsigned char)actual)) {
            size_t inicio = i;
            while (i < longitud &&
                   es_continuacion_identificador((unsigned char)fuente[i])) {
                i++;
            }

            size_t tamano = i - inicio;
            int indice = buscar_palabra(fuente + inicio, tamano);

            if (indice >= 0) {
                conteos[indice]++;
                (*total)++;
                if (traducir) {
                    const char *equivalente = PALABRAS[indice].espanol;
                    buffer_agregar(salida, equivalente, strlen(equivalente));
                } else {
                    buffer_agregar(salida, fuente + inicio, tamano);
                }
            } else {
                buffer_agregar(salida, fuente + inicio, tamano);
            }
            continue;
        }

        buffer_caracter(salida, actual);
        i++;
    }
}

static void imprimir_informe(const char *ruta, const unsigned long *conteos,
                             unsigned long total, FILE *destino) {
    fprintf(destino, "Palabras reservadas encontradas en \"%s\":\n", ruta);

    unsigned long distintas = 0;
    for (size_t i = 0; i < NUM_PALABRAS; i++) {
        if (conteos[i] == 0) {
            continue;
        }
        distintas++;
        fprintf(destino, "  %-14s -> %-18s (%lu ocurrencia%s)\n",
                PALABRAS[i].ingles, PALABRAS[i].espanol, conteos[i],
                conteos[i] == 1 ? "" : "s");
    }

    if (distintas == 0) {
        fprintf(destino, "  No se encontraron palabras reservadas de C.\n");
    } else {
        fprintf(destino,
                "\nTotal: %lu ocurrencia(s) de %lu palabra(s) reservada(s) "
                "distinta(s).\n",
                total, distintas);
    }
}

static void uso(const char *programa) {
    printf("Uso:\n");
    printf("  %s [archivo.c]            Muestra el informe de palabras reservadas\n", programa);
    printf("  %s [archivo.c] --traducir Escribe el codigo traducido en stdout\n", programa);
    printf("  %s -h | --ayuda           Muestra esta ayuda\n", programa);
    printf("\nSi se omite el archivo se usa ejemplo.c\n");
}

int main(int argc, char **argv) {
    const char *ruta = NULL;
    int traducir = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--traducir") == 0 || strcmp(argv[i], "-t") == 0) {
            traducir = 1;
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--ayuda") == 0) {
            uso(argv[0]);
            return 0;
        } else if (ruta == NULL) {
            ruta = argv[i];
        } else {
            fprintf(stderr, "Argumento no reconocido: %s\n", argv[i]);
            uso(argv[0]);
            return 2;
        }
    }

    if (ruta == NULL) {
        ruta = "ejemplo.c";
        fprintf(stderr, "Usando archivo por defecto: %s\n", ruta);
    }

    size_t longitud = 0;
    char *fuente = leer_archivo(ruta, &longitud);

    unsigned long conteos[NUM_PALABRAS] = {0};
    unsigned long total = 0;

    Buffer salida;
    buffer_iniciar(&salida, longitud + 256);

    procesar(fuente, longitud, &salida, conteos, &total, traducir);

    if (traducir) {
        imprimir_informe(ruta, conteos, total, stderr);
        fwrite(salida.datos, 1, salida.longitud, stdout);
    } else {
        imprimir_informe(ruta, conteos, total, stdout);
    }

    buffer_liberar(&salida);
    free(fuente);
    return 0;
}
