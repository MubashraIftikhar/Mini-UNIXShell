#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#define MAX_PROMPT_SIZE 256
#define MAX_HISTORY 10
#define MAX_ARGS 256
#define MAX_VARS 256
#define MAX_VAR_NAME 64
#define MAX_VAR_VALUE 128

typedef struct {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];
    int global;  // 1 for global, 0 for local
} Var;

Var variables[MAX_VARS];
int var_count = 0;

typedef struct {
    pid_t pid;
    char command[MAX_PROMPT_SIZE];
} background_job;

background_job jobs[MAX_HISTORY];
int job_count = 0;

void handle_sigchld(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Remove the job from the jobs list
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid) {
                for (int j = i; j < job_count - 1; j++) {
                    jobs[j] = jobs[j + 1];
                }
                job_count--;
                break;
            }
        }
    }
}

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

void process_help() {
    printf("Built-in commands:\n");
    printf("cd <path>      : Change directory\n");
    printf("exit           : Exit the shell\n");
    printf("jobs           : List background jobs\n");
    printf("kill <job_id>  : Terminate the specified background job\n");
    printf("help           : Display this help message\n");
    printf("set <name> <value> : Set a user-defined variable\n");
    printf("get <name>     : Get the value of a user-defined variable\n");
    printf("list           : List all user-defined variables\n");
    printf("!<number>      : Re-execute a command from history\n");
}

void list_jobs() {
    if (job_count == 0) {
        printf("No background jobs.\n");
    } else {
        for (int i = 0; i < job_count; i++) {
            printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
        }
    }
}

void kill_job(int job_id) {
    if (job_id > 0 && job_id <= job_count) {
        if (kill(jobs[job_id - 1].pid, SIGKILL) == 0) {
            printf("Job %d terminated.\n", job_id);
            for (int i = job_id - 1; i < job_count - 1; i++) {
                jobs[i] = jobs[i + 1];
            }
            job_count--;
        } else {
            perror("kill error");
        }
    } else {
        printf("Invalid job ID.\n");
    }
}

void set_variable(char *name, char *value, int global) {
    // Check if variable already exists
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            strncpy(variables[i].value, value, MAX_VAR_VALUE);
            variables[i].global = global;
            return;
        }
    }

    // Add new variable
    if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, name, MAX_VAR_NAME);
        strncpy(variables[var_count].value, value, MAX_VAR_VALUE);
        variables[var_count].global = global;
        var_count++;
    } else {
        printf("Variable limit reached.\n");
    }
}

void get_variable(char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            printf("%s=%s\n", variables[i].name, variables[i].value);
            return;
        }
    }
    printf("Variable not found.\n");
}

void list_variables() {
    if (var_count == 0) {
        printf("No user-defined variables.\n");
    } else {
        for (int i = 0; i < var_count; i++) {
            printf("%s=%s (%s)\n", variables[i].name, variables[i].value, variables[i].global ? "global" : "local");
        }
    }
}

void process_command(char *input) {
    char *args[MAX_ARGS];
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

    if (args[0] == NULL) {
        return; // Empty input
    }

    // Built-in command checks
    if (strcmp(args[0], "cd") == 0) {
        process_cd_command(args[1]);
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "jobs") == 0) {
        list_jobs();
    } else if (strcmp(args[0], "kill") == 0) {
        if (args[1] != NULL) {
            int job_id = atoi(args[1]);
            kill_job(job_id);
        } else {
            printf("Usage: kill <job_id>\n");
        }
    } else if (strcmp(args[0], "help") == 0) {
        process_help();
    } else if (strcmp(args[0], "set") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            set_variable(args[1], args[2], 0);  // Default to local
        } else {
            printf("Usage: set <name> <value>\n");
        }
    } else if (strcmp(args[0], "get") == 0) {
        if (args[1] != NULL) {
            get_variable(args[1]);
        } else {
            printf("Usage: get <name>\n");
        }
    } else if (strcmp(args[0], "list") == 0) {
        list_variables();
    } else {
        // Forking and executing external commands
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
                    char *next_args[MAX_ARGS];
                    int i = 0;
                    while (token != NULL) {
                        token = strtok(NULL, " ");
                        next_args[i++] = token;
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
        } else if (pid > 0) {  // Parent process
            if (background) {
                printf("[%d] %d\n", ++job_count, pid);
                strncpy(jobs[job_count - 1].command, args[0], MAX_PROMPT_SIZE);
                jobs[job_count - 1].pid = pid;
            } else {
                waitpid(pid, NULL, 0);
            }
        } else {
            perror("fork error");

	}
    }
}
int main() {
    signal(SIGCHLD, handle_sigchld);
    char *input;
    char prompt[MAX_PROMPT_SIZE];

    while (1) {
        display_prompt(prompt);
        input = readline(prompt);
        if (input == NULL) { // EOF detected
            printf("\nExiting shell...\n");
            break;
        }
        if (strlen(input) > 0) {
            add_history(input);
            process_command(input);  // Correct function call
        }
        free(input);  // Free the memory allocated by readline
    }

    return 0; // Proper return statement
}

