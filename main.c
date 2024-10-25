// In main.c
#include <stdio.h>

int main() {
    char input[100];
    while (1) {
        printf("shell> ");
        fgets(input, sizeof(input), stdin);
    }
    return 0;
}

