// main.c
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "executor.h"
#include "prompt.h"

int main() {
    char *cmdline;
    char **arglist;

    while (1) {
        char *prompt = generate_prompt();  // Get dynamic prompt with username and directory
        cmdline = read_cmd(prompt, stdin);
        free(prompt);

        if (cmdline == NULL)  // Exit on EOF (CTRL+D)
            break;

        if ((arglist = tokenize(cmdline)) != NULL) {
            execute(arglist);

            // Free allocated memory
            for (int j = 0; j < MAXARGS + 1; j++) free(arglist[j]);
            free(arglist);
            free(cmdline);
        }
    }
    printf("\n");
    return 0;
}

