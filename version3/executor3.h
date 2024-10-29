// executor3.h

#ifndef EXECUTOR3_H
#define EXECUTOR3_H

#include "parser3.h"  // For CommandInfo struct

// Function to handle SIGCHLD signal
void handle_sigchld(int sig);

// Handles command execution with redirection and piping as per cmd_info
void execute_command(CommandInfo *cmd_info);

#endif // EXECUTOR3_H

