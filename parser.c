//This file contains read_cmd to read commands from input and tokenize to split commands into arguments.
// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static char* history[MAX_HISTORY]; // History array
static int history_count = 0; // Counter for history

void add_to_history(const char* cmdline) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmdline); // Store a copy of the command
    } else {
        // Optionally remove the oldest command or handle overflow
        free(history[0]); // Free the oldest command
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i]; // Shift commands up
        }
        history[MAX_HISTORY - 1] = strdup(cmdline); // Add new command
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    int c;
    int pos = 0;
    char *cmdline = (char*) malloc(sizeof(char) * MAX_LEN);

    while ((c = getc(fp)) != EOF) {
        if (c == '\n')
            break;
        cmdline[pos++] = c;
    }
    if (c == EOF && pos == 0) 
        return NULL;

    cmdline[pos] = '\0';
    return cmdline;
}

char** tokenize(char* cmdline) {
    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int j = 0; j < MAXARGS + 1; j++) {
        arglist[j] = (char*)malloc(sizeof(char) * ARGLEN);
        memset(arglist[j], 0, ARGLEN);
    }

    if (cmdline[0] == '\0')
        return NULL;

    int argnum = 0;
    char* cp = cmdline;
    char* start;
    int len;

    while (*cp != '\0') {
        while (*cp == ' ' || *cp == '\t')
            cp++;
        start = cp;
        len = 1;

        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
            len++;
        
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }
    arglist[argnum] = NULL;
    return arglist;
}

