#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "executor3.h"

#define MAX_ARGS 100

void execute_command(char *cmd) {
    char *args[MAX_ARGS];
    pid_t pid;
    int background = 0;
    int fd;

    // Remove trailing newline
    cmd[strcspn(cmd, "\n")] = 0;

    // Check for background execution
    if (cmd[strlen(cmd) - 1] == '&') {
        background = 1;
        cmd[strlen(cmd) - 1] = 0; // Remove '&' from command
    }

    // Handle I/O Redirection
    char *input_file = NULL;
    char *output_file = NULL;
    char *token = strtok(cmd, " ");
    int i = 0;

    while (token != NULL) {
        // Handle input redirection
        if (strcmp(token, "<") == 0) {
            input_file = strtok(NULL, " ");
        } 
        // Handle output redirection
        else if (strcmp(token, ">") == 0) {
            output_file = strtok(NULL, " ");
        } 
        // Store argument
        else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the arguments array

    // Fork a new process
    pid = fork();
    if (pid == 0) {
        // Child process
        if (input_file) {
            fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("Error opening input file");
                exit(1);
            }
            dup2(fd, STDIN_FILENO); // Redirect input
            close(fd);
        }
        if (output_file) {
            fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); // Redirect output
            close(fd);
        }

        execvp(args[0], args);
        perror("execvp failed"); // Handle exec failure
        exit(1);
    } else if (pid > 0) {
        // Parent process
        static int job_count = 0; // Static variable to keep track of background jobs
        if (!background) {
            waitpid(pid, NULL, 0); // Wait for foreground process
        } else {
            job_count++; // Increment job count for background jobs
            printf("[%d] %d\n", job_count, pid); // Print job number and PID
        }
    } else {
        perror("fork failed"); // Handle fork failure
    }
}

void handle_pipe(char *cmd) {
    char *commands[2];
    char *token = strtok(cmd, "|");
    int i = 0;

    // Split the command by pipe
    while (token != NULL && i < 2) {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL; // Null-terminate the commands array

    if (i == 2) {
        int fd[2];
        pid_t pid1, pid2;

        pipe(fd); // Create a pipe

        // First command
        if ((pid1 = fork()) == 0) {
            dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(fd[0]); // Close unused read end
            execute_command(commands[0]);
            exit(0);
        }

        // Second command
        if ((pid2 = fork()) == 0) {
            dup2(fd[0], STDIN_FILENO); // Redirect stdin from pipe
            close(fd[1]); // Close unused write end
            execute_command(commands[1]);
            exit(0);
        }

        close(fd[0]);
        close(fd[1]);
        waitpid(pid1, NULL, 0); // Wait for first command
        waitpid(pid2, NULL, 0); // Wait for second command
    } else {
        // If not enough commands, execute normally
        execute_command(commands[0]);
    }
}

