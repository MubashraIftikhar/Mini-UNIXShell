#include "parser3.h"
#include "executor3.h"
#include <string.h>

void parse_command(char *cmd) {
    // Handle pipes
    if (strchr(cmd, '|')) {
        handle_pipe(cmd);
    } else {
        // Execute the command normally
        execute_command(cmd);
    }
}

