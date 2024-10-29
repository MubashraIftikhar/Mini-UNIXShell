#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // Include this for chdir
#include "prompt2.h"
#include "parser2.h"
#include "executor2.h"

#define MAX_CMD_LENGTH 1024
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
        if (chdir(path) != 0) {
            perror("cd failed");
        }
        return 1;
    }
    return 0;
}

// Function to retrieve command from history with up-arrow
void retrieve_command(char *cmdline) {
    if (history_count > 0) {
        strcpy(cmdline, history[history_count - 1]);
        printf("%s\n", cmdline);  // Display the retrieved command
    }
}

int main() {
    char cmdline[MAX_CMD_LENGTH];
    int last_char;

    while (1) {
        generate_prompt();  // Display the prompt
        fflush(stdout);

        // Check for EOF (Ctrl+D) to exit the shell
        if (!fgets(cmdline, sizeof(cmdline), stdin)) {
            printf("\nExiting shell...\n");
            break;
        }

        // Remove trailing newline character
        cmdline[strcspn(cmdline, "\n")] = 0;

        // Handle up-arrow key for previous command retrieval
        if (cmdline[0] == '\033') {  // Detects escape sequence
            retrieve_command(cmdline);
            continue;
        }

        // Save command to history if it's not empty
        if (cmdline[0] != '\0') {
            save_to_history(cmdline);
        }

        // Handle "history" command
        if (strcmp(cmdline, "history") == 0) {
            show_history();
            continue;
        }

        // Handle "cd" command
        if (handle_cd(cmdline)) {
            continue;
        }

        // Execute command with redirection and piping
        handle_redirection_and_pipes(cmdline);
    }

    // Free history memory
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}

