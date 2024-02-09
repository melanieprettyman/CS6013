#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "shelpers.h"
using namespace std;

/*NOTES:
 * execvp: This is a system call that replaces the current process with a new process specified
 * by the provided command.
 *  -command.execName.c_str(): converts the execName string to a C-style string, which is required by execvp.
 *  pointer to raw data in the array, its type is const char *---name of program i want to be replaced by
 *  -command.argv.data(): retrieves a pointer to the first element of the underlying array of argv.
 *                        This pointer is needed by execvp to access the array of command-line arguments. --cmd line arguments being passed to replacement process
 *
 */


//Takes the command and execute the command on the parent process
void handleParentChildProcess(const Command& command){
    //Forks the current process to create a child process
    pid_t pid = fork();

    //Checks if the fork failed
    if (pid == -1) {
        cerr << "Fork failed\n";
        exit(EXIT_FAILURE);
    }
        // Child process
    else if (pid == 0) {
        //If the inputFD is not std::in (0), reset STDIN_FILENO to the commands inputFD
        if (command.inputFd != 0) {
            dup2(command.inputFd, STDIN_FILENO);
        }
        //If the inputFD is not std::out (1), reset STDOUT_FILENO to the commands inputFD
        if (command.outputFd != 1) {
            dup2(command.outputFd, STDOUT_FILENO);
        }

        // Execute the command
        //replace this shell with the command process
        // execvp replacing the current process with the new process (execvp takes in a c_str, and char* const* as params)
        execvp(command.execName.c_str(), const_cast<char* const*>(command.argv.data()));
        // If execvp returns, it must have failed
        cerr << "Exec failed\n";
        exit(EXIT_FAILURE);
    }
        // Parent process
    else {
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0); //Waits for the child process with process ID pid to finish and stores its exit status in status

        // Check if child process exited normally
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);

            //If the child process exited with a non-zero status, prints an error message indicating the exit status
            if (exit_status != 0) {
                cerr << "Child process exited with non-zero status: " << exit_status << endl;
            }
        }
            //If the child process was terminated by a signal, prints an error message indicating the signal.
        else if (WIFSIGNALED(status)) {
            cerr << "Child process terminated by signal: " << WTERMSIG(status) << endl;
        }
        //CLOSE child processes
        if (command.inputFd != 0) {
            close(command.inputFd);
        }
        if (command.outputFd != 1) {
            close(command.outputFd);
        }
    }
}

int main() {

    string input;

    //continuously read user input and execute commands until the user exits the shell
    while (true) {
        //Print the shell prompt (>) to indicate that the shell is ready to accept input from the user
        cout << "> ";

        //Reads a line of input from the user and checks if it fails
        if (!getline(cin, input)) {
            // break out of loop and terminate shell
            break;
        }

        //Checks if the user input is "exit".
        if (input == "exit") {
            // break out of loop and terminate shell
            break;
        }

        //Tokenizes the user input into individual tokens
        vector<string> tokens = tokenize(input);

        // If no tokens parsed, continue to next iteration of loop
        if (tokens.empty()) {
            continue;
        }
        // Handling 'cd' as a built-in
        if (tokens[0] == "cd") {
            // Default to HOME directory if no argument is provided, or the next token entered is the address
            string dir = (tokens.size() == 1) ? getenv("HOME") : tokens[1];

            // Attempt to change directory
            if (chdir(dir.c_str()) != 0) {
                // chdir returns -1 on error
                perror("chdir");
            }
            continue; // Skip the rest of the loop and prompt for next command
        }

        //Create commands
        //Parses the tokens into executable commands using the getCommands() function
        vector<Command> commands = getCommands(tokens);

        //Skips to the next iteration of the loop if there are no valid commands
        if (commands.empty()) {
            cerr << "Invalid command\n";
            continue;
        }

        //Pass the commands from the list of commands and execute the command with a child process
        for(const Command& command: commands){
            handleParentChildProcess(command);
        }

    }

    return 0;
}

