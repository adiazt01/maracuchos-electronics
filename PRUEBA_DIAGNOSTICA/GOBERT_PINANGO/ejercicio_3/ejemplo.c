/*
 * Comentario de prueba: int while return for struct enum no deben contarse.
 * Tampoco deben contarse las palabras que aparecen aqui: sizeof, signed.
 */

// Comentario de linea: const char unsigned long register volatile

#include <stdio.h>
#include <stdlib.h>

typedef struct Nodo {
    int valor;
    struct Nodo *siguiente;
} Nodo;

enum Color { ROJO, VERDE, AZUL };

union Dato {
    int entero;
    float flotante;
};

extern int variable_externa;

static const char *LEYENDA =
    "int while return for no cuentan dentro de una cadena";

static unsigned long contar_nodos(const Nodo *nodo) {
    unsigned long total = 0;
    while (nodo != NULL) {
        total += (unsigned long)sizeof(int);
        nodo = nodo->siguiente;
    }
    return total;
}

static double promedio(const short *valores, int cantidad) {
    double suma = 0.0;
    for (int i = 0; i < cantidad; i++) {
        if (valores[i] < 0) {
            continue;
        } else {
            suma += (double)valores[i];
        }
    }
    return suma;
}

static void demo_do(void) {
    int i = 0;
    do {
        i++;
    } while (i < 1);
}

static int demo_goto(int x) {
    if (x < 0) {
        goto error;
    }
    return x;
error:
    return 0;
}

static signed int demo_signed(void) {
    return -1;
}

static int demo_register_volatile(void) {
    register int i = 1;
    volatile int v = 2;
    return i + v;
}

static inline int demo_inline(void) {
    return 0;
}

static int demo_restrict(int *restrict puntero) {
    return *puntero;
}

static _Bool demo_bool(void) {
    return 1;
}

int main(void) {
    Nodo *cabeza = NULL;
    for (int i = 0; i < 3; i++) {
        Nodo *nuevo = (Nodo *)malloc(sizeof(Nodo));
        if (nuevo == NULL) {
            return 1;
        }
        nuevo->valor = i;
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
    }

    short valores[] = {1, -2, 3};
    union Dato dato;
    dato.entero = 7;

    int numero = 5;
    int extra = demo_goto(-1) + demo_signed() + demo_register_volatile() +
                demo_inline() + demo_restrict(&numero) + (int)demo_bool();
    demo_do();

    unsigned long total = contar_nodos(cabeza);
    switch (total) {
        case 0:
            printf("vacio\n");
            break;
        default:
            printf("%s: %lu %.2f %d %d\n", LEYENDA, total,
                   promedio(valores, 3), dato.entero, extra);
            break;
    }

    while (cabeza != NULL) {
        Nodo *siguiente = cabeza->siguiente;
        free(cabeza);
        cabeza = siguiente;
    }

    return 0;
}
