/* Programa de ejemplo: suma los impares hasta n.
   Los comentarios no se traducen: if while for int */
#include <stdio.h>
#include <float.h>

#define LIMITE 10
typedef unsigned int natural;

static natural suma(int n) {
    natural total = 0;
    for (int i = 1; i <= n; i++) {
        if (i % 2 == 0) continue;     // se omiten los pares
        total += i;
    }
    return total;
}

int main(void) {
    const char *texto = "while y if dentro de un texto";
    char letra = 'i';
    int valor = 0x1f, interior = 3, entero_largo = 1;   // estos nombres no se tocan
    switch (valor) {
        case 31: puts("treinta y uno"); break;
        default: puts("otro");
    }
    do { valor--; } while (valor > interior);
    printf("%s %c %u %d\n", texto, letra, suma(LIMITE), entero_largo);
    return 0;
}
