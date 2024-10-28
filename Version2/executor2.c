#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parser2.h"
#include "executor2.h"

#define MAX_ARG_LENGTH 100

void handle_redirection_and_pipes(char *cmdline) {
    char *commands[MAX_ARG_LENGTH];
    int num_commands = parse_pipes(cmdline, commands);

    int pipes[2 * (num_commands - 1)]; // Creating pipes for all commands

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes + i * 2) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Handle input redirection
            if (i > 0) { // Not the first command
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }

            // Handle output redirection
            if (i < num_commands - 1) { // Not the last command
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe fds
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipes[j]);
            }

            // Handle input/output redirection from command arguments
            char *infile = NULL, *outfile = NULL;
            char **args = parse_command(commands[i]);
            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], "<") == 0) {
                    infile = args[j + 1];
                    args[j] = NULL; // Terminate args before '<'
                } else if (strcmp(args[j], ">") == 0) {
                    outfile = args[j + 1];
                    args[j] = NULL; // Terminate args before '>'
                }
            }

            if (infile) {
                int in = open(infile, O_RDONLY);
                if (in == -1) {
                    perror("Error opening input file");
                    exit(1);
                }
                dup2(in, STDIN_FILENO); // Redirect stdin
                close(in);
            }

            if (outfile) {
                int out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out == -1) {
                    perror("Error opening output file");
                    exit(1);
                }
                dup2(out, STDOUT_FILENO); // Redirect stdout
                close(out);
            }

            // Execute the command
            if (execvp(args[0], args) == -1) {
                perror("Exec failed");
                exit(1);
            }
        }
    }

    // Close all pipe fds in the parent process
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipes[i]);
    }

    // Parent process waits for all child processes
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

