#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string.h>

void handle_redirection(char *cmd) {
    char *infile = NULL, *outfile = NULL;
    char **args = parse_command(cmd);

    // Look for redirection symbols in args
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            infile = args[i + 1];
            args[i] = NULL; // Terminate args before '<'
        } else if (strcmp(args[i], ">") == 0) {
            outfile = args[i + 1];
            args[i] = NULL; // Terminate args before '>'
        }
    }

    if (infile) {
        int in = open(infile, O_RDONLY);
        if (in == -1) {
            perror("Error opening input file");
            exit(1);
        }
        dup2(in, STDIN_FILENO);
        close(in);
    }

    if (outfile) {
        int out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out == -1) {
            perror("Error opening output file");
            exit(1);
        }
        dup2(out, STDOUT_FILENO);
        close(out);
    }

    if (execvp(args[0], args) == -1) {
        perror("Exec failed");
        exit(1);
    }
}

