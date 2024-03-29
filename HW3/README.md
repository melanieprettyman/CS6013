
# Custom Unix Shell 

This project implements a simple shell in C++, similar in function to bash or zsh, but more limited in scope. It can execute single commands, handle I/O redirection, background processes, implement pipelines, and execute built-in commands like cd. This shell utilizes POSIX system calls like fork(), exec(), dup2(), and pipe() to manage processes and I/O.





## Features 
**Single Command Execution:**
- Executes a single command at a time.
- Continuously reads commands from the standard input until "exit" is entered or an EOF is received (Ctrl+D).

**I/O Redirection**
- Supports simple input (<) and output (>) redirection.
- Commands like ```echo "Hello World" > outfile.txt``` and ```cat < infile.txt``` are supported.

**Pipelines**
- Implements command pipelining using the pipe (|) symbol.
- For example, cat file.txt | grep "search term" | sort will work as expected.

**Shell Builtins**
- Currently supports the `cd` command as a built-in for changing the current working directory.

**Additional Features**
- Background execution: Append `&` to a command to run it in the background.
- Checks and reports the completion of background processes before prompting for the next command.








## Setup and Installation

Clone the repository and compile project with make file. 
Run the shell:
`./myshell`

## API Reference 

| Function                      | Parameter                              | Returns         |
| ----------------------------- | ---------------------------------------- | -------------|
| `executeChildProcess`         | `const Command& command` | void (Exits on failure) |
| `handleParentProcess`   | `pid_t pid, const Command& command, std::vector<pid_t>& backgroundProcesses` | void |
| `executeCommandProcess`       | `const Command& command, std::vector<pid_t>& backgroundProcesses` | void |
| `checkBackgroundProcesses`    | `std::vector<pid_t>& backgroundProcesses` | void |
| `changeDirectory`             | `const std::vector<std::string>& tokens` | void |
| `tokenize`                    | `const string& s` | `vector<string>` |
| `getCommands`                 | `const vector<string>& tokens` | `vector<Command>` |
| `closeCommandFileDescriptors` | `Command& command` | void |








## Demo

![App Screenshot](https://github.com/melanieprettyman/CS6013/blob/main/HW3/Terminal_Demo.png?raw=true)


## Structures
`Command`
Represents a command to be executed by the shell.

### Members
- **execName:** Name of the executable (e.g., "ls", "cat").
- **argv:** Argument vector for the command.
- **inputFd:** File descriptor for input redirection.
- **outputFd:** File descriptor for output redirection.
- **background:** Boolean indicating if the command should be run in the background.




## Acknowledgements

The Custom Unix Shell project was created by Melanie Prettyman and was developed as part of CS 6013 Operating Systems. 

We also appreciate the guidance and support provided by Professor Dav de St. Germain throughout the project.

