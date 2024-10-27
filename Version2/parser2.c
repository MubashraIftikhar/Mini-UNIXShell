#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define MAX_TOKENS 100

// Function to split a command into tokens by whitespace
char **parse_command(char *command) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    char *token;
    int position = 0;

    token = strtok(command, " \t\n");
    while (token != NULL) {
        tokens[position++] = token;
        token = strtok(NULL, " \t\n");
    }
    tokens[position] = NULL;
    return tokens;
}

// Function to split commands by the pipe character and store them in arrays
int parse_pipes(char *cmdline, char **cmds) {
    int position = 0;
    char *pipe_segment = strtok(cmdline, "|");

    while (pipe_segment != NULL) {
        cmds[position++] = pipe_segment;
        pipe_segment = strtok(NULL, "|");
    }
    cmds[position] = NULL;
    return position;
}

