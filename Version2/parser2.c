#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser2.h"

#define MAX_ARG_LENGTH 100

char **parse_command(char *cmdline) {
    char **args = malloc(MAX_ARG_LENGTH * sizeof(char *));
    char *arg = strtok(cmdline, " ");
    int index = 0;

    while (arg != NULL) {
        args[index++] = arg;
        arg = strtok(NULL, " ");
    }
    args[index] = NULL; // Null terminate the argument list
    return args;
}

int parse_pipes(char *cmdline, char **commands) {
    int position = 0;
    char *pipe_segment = strtok(cmdline, "|");

    while (pipe_segment != NULL) {
        commands[position++] = pipe_segment;
        pipe_segment = strtok(NULL, "|");
    }
    commands[position] = NULL; // Null terminate the commands array
    return position; // Return number of commands
}

