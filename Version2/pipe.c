#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include "parser.h"

// Function to handle commands with a pipe
void handle_pipe(char *cmd1, char *cmd2) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return;
    }

    // First child process (cmd1)
    if ((p1 = fork()) == 0) {
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[0]); // Close unused read end of the pipe
        close(pipefd[1]);

        char **args1 = parse_command(cmd1);
        if (execvp(args1[0], args1) == -1) {
            perror("Exec failed for cmd1");
            exit(1);
        }
    }

    // Second child process (cmd2)
    if ((p2 = fork()) == 0) {
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[1]); // Close unused write end of the pipe
        close(pipefd[0]);

        char **args2 = parse_command(cmd2);
        if (execvp(args2[0], args2) == -1) {
            perror("Exec failed for cmd2");
            exit(1);
        }
    }

    // Parent process: Close both ends of the pipe and wait for children
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

