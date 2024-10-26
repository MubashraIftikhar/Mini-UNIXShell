//This file contains read_cmd to read commands from input and tokenize to split commands into arguments.
// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

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

