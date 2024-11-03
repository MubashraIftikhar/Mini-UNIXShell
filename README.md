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
## Dependencies
Shell is written in C and relies on the following:

* GCC compiler
* Readline library.
* A Unix-based operating system (Linux or MacOS).

## Version01
<br>**USAGE**<br> 
<pre> gcc -o myshellv1 main.c parser.c executor.c prompt.c </pre>
<pre> ./myshellv1 </pre>
  <br>This version contain multiple .c files that are as follows
  * **Prompt.c**: This contains function to generate prompt
  * **Parser.c**: This file contains read_cmd function to read commands from input and tokenize to split commands into arguments.
  * **Executor.c**: This file contains function that forks a new process using fork().In the child process, it attempts to execute the command using execvp()
  * **Main.c**
 <br> **FEATURES:** <br>
    Overall The shell generates a dynamic prompt that displays the username, hostname, and current working directory. <br>The format of the prompt is: ```PUCITshell:username@hostname:current_directory$``` . <br>We can retrieve Present working directory using **pwd**  <br>Also we can create , remove and list directory using (mkdir,rmdir,ls) respectively.
We can exit shell using Ctrl-D

## Version02
<br>**USAGE**<br> 
<pre> gcc -o myshell main2.c parser2.c executor2.c prompt2.c </pre>
<pre>./myshell</pre>
  <br>This version contain multiple .c files that are as follows
  * **Prompt2.c**: This contains function to generate prompt
  * **Parser2.c**: Theis files handle the parsing of user commands, splitting input lines into arguments and parsing pipe-separated commands.
  * **Executor2.c**: These files manage the execution of commands, including handling piping, redirection, and executing the parsed commands in child processes.
  * **Main2.c**:  This file contains the core logic of the shell program, managing user input, history tracking, and the overall flow of command execution
<br> **FEATURES:** <br>
    This shell contain feautures from Version1.<br>
    Enhanced Features contain Call to handle_redirection_and_pipes() to process and execute non-builtin commands.<br>
    Checks for built-in commands (cd,history)

   **Errors Faced** <br>
     execvp() Failure

## Version03
<br>**USAGE**<br> 
<pre> gcc -o myshellv3 main3.c parser3.c executor3.c prompt3.c </pre>
  <br>This version contain multiple .c files that are as follows
  * **Prompt.c**: This contains function to generate prompt
  * **Parser.c**:  Contains logic for parsing commands 
  * **Executor3.c**: Handles command execution, including background processes, I/O redirection, and pipes.
  * **Main.c**
 <br> **FEATURES:** <br>
    Overall The shell containt functionality for signal handling.
    <br>Placing commands (external only) in the background
    <br>Avoid Zombies
## Version04
<br>**USAGE**<br> 
<pre> gcc -o myshellv4 version4.c -lreadline </pre>
<pre> ./myshellv4 </pre>
