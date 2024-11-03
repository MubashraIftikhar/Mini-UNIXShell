# Creating UNIX Shell
## Introduction
This repository houses the Shell project,designed to replicate the basic functionalities of a Unix shell. This  assignment aims to practice in the use of UNIX system calls and writing a command interpreter on top
of UNIX. It offers an opportunity to implement a custom shell that can parse and execute commands, handle environment variables,background processed and support pipes and redirections.
## Features 
My repository contains 6 versions, each one building on the previous version with incremental improvements and feature enhancements. Starting from a basic shell implementation, subsequent versions progressively introduce new functionalities such as  built-in commands, pipe redirection,background processedand command history. This structured approach demonstrates the step-by-step development and refinement of a custom shell, showcasing the evolution of its features and capabilities over time.
  * Parsing and execution of shell commands.
  * Built-in commands implementation (echo, cd, pwd, env, exit).
  * Environment variable management.
  * Support for pipes (|) and redirections (> , <).
  * Signal handling (Ctrl-D).

## Version01
TO COMPILE: gcc -o myshellv1 main.c parser.c executor.c prompt.c
  This version contain multiple .c files that are as follows
  * Prompt.c: This contains function to generate prompt
  * Parser.c: This file contains read_cmd function to read commands from input and tokenize to split commands into arguments.
  * Executor.c: This file contains function that forks a new process using fork().In the child process, it attempts to execute the command using execvp()
  * Main.c
  *  **FEATURES:**
    Overall The shell generates a dynamic prompt that displays the username, hostname, and current working directory.The format of the prompt is: **PUCITshell:username@hostname:current_directory$** .We can retrieve Present working directory using **pwd** Also we can create , remove and list directory using (mkdir,rmdir,ls) respectively.
We can exit shell using Ctrl-D

## Version2
