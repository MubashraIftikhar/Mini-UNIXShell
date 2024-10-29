// prompt3.c
#include "prompt3.h"

void generate_prompt() {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    
    if (pw == NULL) {
        perror("Error retrieving username");
        return;
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Error retrieving hostname");
        strcpy(hostname, "unknown_host");
    }

    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error retrieving current working directory");
        strcpy(cwd, "/");
    }

    printf("PUCITshell:%s@%s:%s$ ", pw->pw_name, hostname, cwd);
    fflush(stdout);
}

