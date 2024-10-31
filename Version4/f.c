#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_PROMPT_SIZE 256
#define MAX_HISTORY 10

void display_prompt(char *prompt) {
    char cwd[MAX_PROMPT_SIZE - 20];  // Reserve space for the prompt prefix
    getcwd(cwd, sizeof(cwd) - 1);    // Limit cwd to prevent truncation
    snprintf(prompt, MAX_PROMPT_SIZE, "PUCITshell@%.230s: ", cwd);
}

void process_cd_command(char *path) {
    if (chdir(path) != 0) {
        perror("cd error");
    }
}

void process_command(char *input) {
    // Tokenize the input for processing
    char *args[256];
    char *token = strtok(input, " ");
    int arg_count = 0;
    int background = 0;
    int in_redirect = 0, out_redirect = 0;
    char *infile = NULL, *outfile = NULL;
    int pipe_fds[2];
    int pipe_used = 0;

    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            infile = token;
            in_redirect = 1;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            outfile = token;
            out_redirect = 1;
        } else if (strcmp(token, "|") == 0) {
            pipe_used = 1;
            break; // Exit the loop at the first pipe
        } else if (strcmp(token, "&") == 0) {
            background = 1;
        } else {
            args[arg_count++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;

    if (strcmp(args[0], "cd") == 0) {
        process_cd_command(args[1]);
    } else {
        pid_t pid = fork();
        if (pid == 0) {  // Child process
            if (in_redirect) {
                int fd_in = open(infile, O_RDONLY);
                if (fd_in == -1) {
                    perror("Error opening input file");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (out_redirect) {
                int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out == -1) {
                    perror("Error opening output file");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            if (pipe_used) {
                pipe(pipe_fds);
                if (fork() == 0) {  // First command process
                    dup2(pipe_fds[1], STDOUT_FILENO);
                    close(pipe_fds[0]);
                    execvp(args[0], args);
                    perror("exec error");
                    exit(EXIT_FAILURE);
                } else {  // Parent process of the first command
                    dup2(pipe_fds[0], STDIN_FILENO);
                    close(pipe_fds[1]);

                    // Prepare arguments for the second command
                    char *next_args[256];
                    int i = 0;
                    while (token != NULL) {
                        token = strtok(NULL, " ");
                        if (token != NULL) {
                            next_args[i++] = token;
                        }
                    }
                    next_args[i] = NULL;

                    execvp(next_args[0], next_args);
                    perror("exec error");
                    exit(EXIT_FAILURE);
                }
            } else {
                execvp(args[0], args);
                perror("exec error");
                exit(EXIT_FAILURE);
            }
        } else {  // Parent process
            if (!background) {
                waitpid(pid, NULL, 0);
            } else {
                printf("[1] %d\n", pid);
            }
        }
    }
}

void handle_history_commands(char *input) {
    if (input[0] == '!') {
        int cmd_num = 0;
        if (input[1] == '-') {
            cmd_num = -1;
        } else {
            cmd_num = atoi(input + 1);
        }

        HIST_ENTRY **history_entries = history_list();
        int history_length = history_length;

        if (cmd_num == -1 && history_length > 0) {
            strcpy(input, history_entries[history_length - 1]->line);
        } else if (cmd_num > 0 && cmd_num <= history_length) {
            strcpy(input, history_entries[cmd_num - 1]->line);
        } else {
            fprintf(stderr, "No such command in history.\n");
            input[0] = '\0';
        }
    }
}

int main() {
    char *input;
    char prompt[MAX_PROMPT_SIZE];

    while (1) {
        display_prompt(prompt);
        input = readline(prompt);

        if (input == NULL) {
            printf("exit\n");
            break;
        }

        if (strlen(input) > 0) {
            handle_history_commands(input);

            if (input[0] != '\0') {
                add_history(input);
                process_command(input);
            }
        }

        free(input);  // Clean up the input buffer
    }

    return 0;
}

