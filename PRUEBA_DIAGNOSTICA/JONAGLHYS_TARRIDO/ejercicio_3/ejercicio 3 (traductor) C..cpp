#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *c_word;
    const char *es_translation;
    int contador; 
} KeywordMapping;

KeywordMapping keywords[] = {
    {"auto", "automático", 0}, {"break", "interrumpir", 0}, {"case", "caso", 0},
    {"char", "carácter", 0}, {"const", "constante", 0}, {"continue", "continuar", 0},
    {"default", "por defecto", 0}, {"do", "hacer", 0}, {"double", "doble", 0},
    {"else", "si no", 0}, {"enum", "enumeración", 0}, {"extern", "externo", 0},
    {"float", "flotante", 0}, {"for", "para", 0}, {"goto", "ir a", 0},
    {"if", "si", 0}, {"int", "entero", 0}, {"long", "largo", 0},
    {"register", "registro", 0}, {"return", "retornar", 0}, {"short", "corto", 0},
    {"signed", "con signo", 0}, {"sizeof", "tamaño de", 0}, {"static", "estático", 0},
    {"struct", "estructura", 0}, {"switch", "seleccionar", 0}, {"typedef", "definición de tipo", 0},
    {"union", "unión", 0}, {"unsigned", "sin signo", 0}, {"void", "vacío", 0},
    {"volatile", "volátil", 0}, {"while", "mientras", 0}
};
int total_kw = sizeof(keywords) / sizeof(KeywordMapping);

void contar_coincidencia(const char *word) {
    for (int i = 0; i < total_kw; i++) {
        if (strcmp(keywords[i].c_word, word) == 0) {
            keywords[i].contador++;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *file = fopen(__FILE__, "r");
    if (!file) {
        printf("No se pudo abrir el archivo fuente actual.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc((size + 1) * sizeof(char));
    if (!buffer) {
        fclose(file);
        return 1;
    }

    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    fclose(file);

    printf("=== RESUMEN DE PALABRAS RESERVADAS EN C ===\n\n");
    
    char palabra[50];
    int p_idx = 0;

    for (long i = 0; i <= size; i++) {
        char c = buffer[i];
        if (isalnum(c) || c == '_') {
            if (p_idx < 49) palabra[p_idx++] = c;
        } else {
            if (p_idx > 0) {
                palabra[p_idx] = '\0';
                contar_coincidencia(palabra);
                p_idx = 0;
            }
        }
    }

    FILE *out = fopen("resultado_ejercicio3.txt", "w");
    if (out) {
        fprintf(out, "--- RESULTADOS EJERCICIO 3 (Resumen de Palabras Reservadas) ---\n");
    }

    for (int i = 0; i < total_kw; i++) {
        if (keywords[i].contador > 0) {
            printf("La palabra '%s' (%s) aparecio %d veces.\n", 
                   keywords[i].c_word, keywords[i].es_translation, keywords[i].contador);
            if (out) {
                fprintf(out, "'%s' (%s): %d veces\n", 
                        keywords[i].c_word, keywords[i].es_translation, keywords[i].contador);
            }
        }
    }

    if (out) fclose(out);
    free(buffer);
    printf("\nAnalisis terminado. Resultados guardados en 'resultado_ejercicio3.txt'.\n");
    return 0;
}