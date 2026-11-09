#include <stdio.h>

// Comentario que no cambia
int interior = 0;

int main(void) {
    char letra = 'i';
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            printf("while int return\n");
        } else {
            interior += i;
        }
    }
    return 0;
}
