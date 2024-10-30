#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "parser3.h"
#include "prompt.h"

#define MAX_CMD_LEN 1024
#define HISTORY_SIZE 100

char *history[HISTORY_SIZE];
int history_count = 0;

// Function to save commands to history
void save_to_history(const char *cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    }
}

// Function to display command history
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

// Function to handle "cd" command
int handle_cd(const char *cmd) {
    if (strncmp(cmd, "cd ", 3) == 0) {
        const char *path = cmd + 3;
        char *modifiable_path = strdup(path);
        if (modifiable_path == NULL) {
            perror("strdup failed");
            return 0;
        }
        modifiable_path[strcspn(modifiable_path, "\n")] = 0; // Remove newline if present

        if (chdir(modifiable_path) != 0) {
            perror("cd failed");
        }
        free(modifiable_path);
        return 1; // Command was a "cd"
    }
    return 0; // Not a "cd" command
}

// Signal handler for SIGCHLD
void handle_sigchld(int sig) {
    // Reap all dead processes to avoid zombies
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    char cmd[MAX_CMD_LEN];

    // Set up the signal handler for SIGCHLD
    signal(SIGCHLD, handle_sigchld);

    while (1) {
        generate_prompt(); // Display the prompt
        if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
            break; // Handle EOF
        }

        // Save command to history
        save_to_history(cmd);

        // Handle exit command
        if (strcmp(cmd, "exit\n") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Handle history command
        if (strcmp(cmd, "history\n") == 0) {
            show_history();
            continue;
        }

        // Handle cd command
        if (handle_cd(cmd)) {
            continue; // Skip command parsing for cd
        }

        // Parse and execute the command
        parse_command(cmd);
    }

    // Free allocated memory for command history
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}

