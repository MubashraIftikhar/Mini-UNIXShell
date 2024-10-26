//executor.c
//This file handles executing commands with execvp and managing the child process.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"

int execute(char* arglist[]) {
    int status;

    // Check if the command is "cd"
    if (strcmp(arglist[0], "cd") == 0) {
        // If no directory is specified, change to the home directory
        if (arglist[1] == NULL) {
            arglist[1] = getenv("HOME"); // Default to home directory if none is provided
        }
        
        // Attempt to change directory
        if (chdir(arglist[1]) != 0) {
            perror("cd failed");
        }
        return 0;  // No need to fork a new process for `cd`
    }

    // Handle other commands using fork and execvp
    int cpid = fork();
    switch (cpid) {
        case -1:
            perror("fork failed");
            exit(1);
        case 0:
            execvp(arglist[0], arglist);
            perror("Command not found...");
            exit(1);
        default:
            waitpid(cpid, &status, 0);
            printf("child exited with status %d\n", status >> 8);
            return 0;
    }
}

