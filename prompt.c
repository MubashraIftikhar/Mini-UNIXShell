// prompt.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <string.h>
#include "prompt.h"

char* generate_prompt() {
    char cwd[PATH_MAX];
    struct passwd *pws = getpwuid(getuid());
    getcwd(cwd, sizeof(cwd));

    char* prompt = (char*)malloc(sizeof(char) * (strlen(pws->pw_name) + strlen(cwd) + 20));
    snprintf(prompt, PATH_MAX + 50, "PUCITshell:%s@%s$ ", pws->pw_name, cwd);

    return prompt;
}

