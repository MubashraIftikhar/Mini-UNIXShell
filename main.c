// main.c
// main.c
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include "parser.h"
#include "executor.h"
#include "prompt.h"

int main() {
    char *cmdline;
    char **arglist;

    while (1) {
        char *prompt = generate_prompt();  
        cmdline = read_cmd(prompt, stdin);
        free(prompt);

        if (cmdline == NULL)  
            break;

        // Check for 'history' command
        if (strcmp(cmdline, "history") == 0) {
            print_history(); // Print command history
        } else {
            add_to_history(cmdline); // Add command to history

            if ((arglist = tokenize(cmdline)) != NULL) {
                execute(arglist);

                // Free allocated memory
                for (int j = 0; j < MAXARGS + 1; j++) free(arglist[j]);
                free(arglist);
            }
        }

        free(cmdline);
    }
    printf("\n");
    return 0;
}

