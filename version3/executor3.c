// executor3.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parser3.h"
#include "executor3.h"

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void execute_command(CommandInfo *cmd_info) {
    int i;
    int pipefds[2 * (cmd_info->num_commands - 1)];
    pid_t pid;

    // Set up pipes for commands if there are multiple commands
    for (i = 0; i < cmd_info->num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe failed");
            exit(1);
        }
    }

    // Iterate over commands and fork processes
    for (i = 0; i < cmd_info->num_commands; i++) {
        pid = fork();
        if (pid == 0) { // Child process
            // Handle redirection
            if (i == 0 && cmd_info->infile) {
                int fd_in = open(cmd_info->infile, O_RDONLY);
                if (fd_in < 0) perror("open infile");
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            if (i == cmd_info->num_commands - 1 && cmd_info->outfile) {
                int fd_out = open(cmd_info->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) perror("open outfile");
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            // Pipe handling
            if (i > 0) { // Connect to previous command's output
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < cmd_info->num_commands - 1) { // Connect to next command's input
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (cmd_info->num_commands - 1); j++) {
                close(pipefds[j]);
            }

            // Execute command
            char *args[MAX_ARGS];
            parse_command(cmd_info->commands[i], args);
            if (execvp(args[0], args) == -1) {
                perror("execvp failed");
                exit(1);
            }
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }

    // Close all pipe file descriptors in the parent
    for (i = 0; i < 2 * (cmd_info->num_commands - 1); i++) {
        close(pipefds[i]);
    }

    // Background job handling
    if (cmd_info->background) {
        printf("[1] %d\n", pid);
    } else {
        // Wait for all child processes if running in foreground
        for (i = 0; i < cmd_info->num_commands; i++) {
            wait(NULL);
        }
    }
}

