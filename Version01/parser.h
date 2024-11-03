// parser.h
#ifndef PARSER_H
#define PARSER_H

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define MAX_HISTORY 100  // Limit for the history

char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
void add_to_history(const char* cmdline);
void print_history();

#endif

