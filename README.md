# Shell_Project

## Overview
This is a custom shell implementation in C, designed to provide basic shell functionality including command execution, piping, input/output redirection, and background processes.

## Features
- Command execution
- Piping (`|`)
- Input redirection (`<`)
- Output redirection (`>`)
- Combined input and output redirection
- Background processes (`&`)
- Signal handling for background processes
- Trimming of whitespace and removal of quotes from input
- Error handling for unsupported redirection operators (`<<` and `>>`)

## Compilation
To compile the shell, use the following command:
gcc -o my_shell my_shell.c

## Usage
Run the shell using:
./my_shell

To run the shell without printing a new line after each command, use:
./my_shell -n

## Command Syntax
- Basic command: `command [args]`
- Piping: `command1 | command2`
- Input redirection: `command < input_file`
- Output redirection: `command > output_file`
- Combined redirection: `command < input_file > output_file`
- Background process: `command &`

## Examples
1. List files: `ls -l`
2. Pipe output: `ls -l | grep ".txt"`
3. Input redirection: `sort < unsorted.txt`
4. Output redirection: `ls -l > file_list.txt`
5. Combined redirection: `sort < unsorted.txt > sorted.txt`
6. Background process: `sleep 10 &`

## Limitations
- The `<<` (here document) and `>>` (append) operators are not supported and will result in an error message.
- Maximum command length is limited to 512 characters.
- Maximum number of piped commands is limited to 100.

## Error Handling
- The shell will print appropriate error messages for issues such as command not found, file not found, and permission denied.
- Unsupported redirection operators (`<<` and `>>`) will result in an error message, and the command will be skipped.

## Signal Handling
- The shell implements a signal handler for `SIGCHLD` to properly manage completed background processes.

## Memory Management
- The shell allocates memory dynamically for parsing and storing commands.
- Memory is freed appropriately to prevent leaks.

## Future Improvements
- Implement support for more advanced shell features like command history, tab completion, and shell built-ins.
- Enhance error handling and user feedback.
- Implement proper quoting and escaping rules.


# Project1 minideadline:

In this code, I focused on parsing the input from the user. I created two string functions, one to parse the input based on the character "|", and stored this in a temporary character array of arrays. Then, I would call the second "parse_command" function that would parse the string based on spaces or " ". 

Because this deadline only needed the call execvp on the first command, I did that. I made sure to call "parse_command" on the first command that the user inputted, and then used the template from class to create a fork -> execvp and waitpid to execute the command. 

Some issues I ran into was parsing a string in C. I found it quite annoying to end each string with a '\0' and also making sure my arguments ended with NULL. Without the NULL, the execvp didn't seem to work.

# Full Deadline ReadMe: 
The goal of this project was to implement a basic shell in C that mimics the behavior of traditional Unix shells. The shell supports executing commands, handling input/output redirection, creating pipelines between commands, and running processes in the background. It follows a typical REPL (Read-Eval-Print Loop) pattern where the shell displays a prompt, processes user input, executes commands, and then waits for further input.

To do this, I first parsed the commands (I did this in the mini-deadline) but also introduced more parsing to incorporate the addition of redirection in my code. I had to support input and output redirection. 


My code works as follows:
- Parse commands, find the read/write files, determine redirection, number of commands, and other needed commands. 
- First open a pipe, or open a read only file if the read redirection is detected. In my code, this is described by redirection == 1. However, you also redirect if input AND output redirection occur on the same line (== 3 || == 4). - Once opened, we utilized dup2 to close the existing file process and copy the file discriptor into the pipe or the opened file. 
- I then close the files in both the parent and child processes
- Piping works by connecting the output of one command to the input of the next command. 
- Then run execvp on the command, getting rid of any redirection characters and write files.
- Finally, I implemented a background variable to detect whether a '&' is found. Instead of calling waitpid in the parent process, if the '&' is found, the process would not wait. For example, sleep 5 & will execute sleep in the background, allowing the shell to immediately accept new commands without waiting for the sleep command to complete. This is implemented by forking a child process and not waiting for it in the parent process. 
- The shell also handles SIGCHLD signals to clean up completed background processes and prevent zombie processes.


In the second part of the assignment, I struggled a lot initially with learning how execvp worked with the read and write files. I initially created a system of redirection that allowed for just redirection, or just regular piping. I created multiple if, else if statements, but I later realized that piping and redirection (at the same time) would be an issue. 

Some of the issue came with parsing and getting the read and write file, but after I realized that the read or write file came directly after a '<' or a '>' symbol, I was able to find the files after some time. To open the corresponding file, I had to read the man page for open, and was able to create a suitible read and write file(using RDONLY, WRONLY, OTRUNC, etc). Similar to a pipe, I had to close the initial opened file after dup2'ing it. 


One key challenge I encountered was differentiating how piping should be handled for the first command, middle commands, and the last command in a pipeline.
- For the first command: We do not need to dup2() the stdin because the input comes directly from the user (via the terminal or redirected from a file), not from a previous command's output. We only need to redirect the output (stdout) to the write end of the pipe so that the next command can read from it.
- For the middle commands: These commands are both receiving input from a previous command and sending output to the next command in the pipeline. Therefore, we need to dup2() both the input (stdin) from the read end of the previous pipe and the output (stdout) to the write end of the next pipe.
- For the last command: Similar to the first command, we do not need to dup2() the stdout, because the output should go directly to the terminal (or to a file if redirected), not to another command. However, we do need to redirect the input (stdin) to read from the pipe's output (which comes from the previous command).

The hint about closing the file in both the child and parent helped my code work. I close any file that I dup and also close the previous pipes in the parent.

One of the most challenging aspects was managing background processes, particularly handling the SIGCHLD signal. When a process finishes in the background, it sends a SIGCHLD signal, which must be properly caught by the parent shell. I used the sigaction() function to manage this signal and ensure that the shell does not leave behind zombie processes. To add on, my code didn't work without blocking the system call before forking. I would set the system call again in the parent process if I am not waiting, but this helped for unwanted system calls from sigchld. 