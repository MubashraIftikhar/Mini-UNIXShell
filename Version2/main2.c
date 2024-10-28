#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt2.h"
#include "parser2.h"
#include "executor2.h"

#define MAX_CMD_LENGTH 1024

int main() {
    char cmdline[MAX_CMD_LENGTH];

    while (1) {
        generate_prompt();  // Display the prompt
        fflush(stdout);

        // Read input command
        if (!fgets(cmdline, sizeof(cmdline), stdin)) {
            break; // Exit on EOF
        }

        // Remove trailing newline character
        cmdline[strcspn(cmdline, "\n")] = 0;

        // Handle pipes and redirection
        handle_redirection_and_pipes(cmdline);
    }

    return 0;
}

