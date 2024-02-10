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
 *                        This pointer is needed by execvp to access the cmd line arguments being passed to replacement process
 *
 * WNOHANG: flag makes waitpid return immediately if the process identified by the PID has not yet completed,
            allowing the function to check the process status without blocking.
 */


std::string getDavPortrait();

/**
 * Executes the command in the child process
 *
 * This function is called after a fork in the child process. It sets up the necessary file
 * descriptors for standard input and output redirection according to the command.
 * After setting up the redirection, it executes the command using execvp. If execvp fails,
 * it prints an error message and exits the child process with a failure status.
 *
 * @param command The command to be executed, including its arguments and file descriptors
 *                for input and output redirection.
 */

void executeChildProcess(const Command& command) {
    //If the inputFD is not std::in (0), reset STDIN_FILENO to the commands inputFD
    if (command.inputFd != 0) {
        dup2(command.inputFd, STDIN_FILENO);
    }

    //If the outputFD is not std::out (1), reset STDOUT_FILENO to the commands inputFD
    if (command.outputFd != 1) {
        dup2(command.outputFd, STDOUT_FILENO);
    }

    //**see note above for execvp**
    // Execute the command by replacing this shell with the command process
    execvp(command.execName.c_str(), const_cast<char *const *>(command.argv.data()));

    // If execvp returns something, it means it failed
    cerr << "Exec failed\n";
    exit(EXIT_FAILURE);
}

/**
 * Manages the parent process's actions post-fork, handling both direct and background processes.
 *
 * In the case of a direct process, this function waits for the child process to finish
 * and checks its exit status. It handles the exit status by printing out appropriate messages
 * for normal exit or termination by a signal.
 *
 * For background processes, the child's PID is added to a vector to be monitored without waiting,
 * allowing the shell to immediately return to the prompt and accept new commands.
 *
 * This function also closes any file descriptors opened for redirection.
 *
 * @param pid The process ID of the child process.
 * @param command The command being executed, used to determine if it's a background process.
 * @param backgroundProcesses A reference to a vector storing the PIDs of background processes.
 */
void handleParentProcess(pid_t pid, const Command& command, std::vector<pid_t>& backgroundProcesses) {
    //Check if the command is a background commands, if so do not wait for child process to finish
    if (command.background) {
        backgroundProcesses.push_back(pid); // Add background process to vector
        cout << "Started background process ID: " << pid << endl;
    } else {
        // Wait for the child process to finish and stores its exit status in status
        int status;
        waitpid(pid, &status,0);

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
        //CLOSE child processes for non-standard FD's
        if (command.inputFd != 0) {
            close(command.inputFd);
        }
        if (command.outputFd != 1) {
            close(command.outputFd);
        }
    }
}

/**
 * Forks the current process to execute a given command, invoking to child or parent handlers.
 *
 * This function forks the shell process. The child process executes the command using `executeChildProcess`,
 * while the parent process manages the execution status and background processes tracking with `handleParentProcess`.
 *
 * @param command Contains command details including executable, arguments, and redirection info.
 * @param backgroundProcesses Tracks background processes' PIDs for the parent process.
 */
void executeCommandProcess(const Command& command, std::vector<pid_t>& backgroundProcesses) {
    //Forks the current process to create a child process
    pid_t pid = fork();

    //Checks if the fork failed
    if (pid == -1) {
        cerr << "Fork failed\n";
        exit(EXIT_FAILURE);
    }

    //CHILD PROCESS
    else if (pid == 0)
    {
        executeChildProcess(command);
    }
    //PARENT PROCESS
    else {
        handleParentProcess(pid, command, backgroundProcesses);
    }
}


//**See note above for WNOHANG**
//Function loops over each process pid in vector storing background-processes and check if the pid has completed yet.
//If it's completed, print out a message saying so.
void checkBackgroundProcesses(std::vector<pid_t>& backgroundProcesses) {
    for(int i = 0; i < backgroundProcesses.size(); i++){
        int status;
        pid_t result = waitpid(i, &status, WNOHANG);
        //If waitpid returns a positive number, the process that been completed and should be removed from the vector
        if (result > 0) {
            cout << "Background process " << backgroundProcesses[i] << " completed." << endl;
            backgroundProcesses.erase(backgroundProcesses.begin() + i);
        }
    }
}

/**
 * This function changes the current working directory of the shell to the one specified in the command arguments.
 * If no directory is specified, it defaults to changing to the user's home directory. If the directory change fails,
 * it reports an error.
 *
 * @param tokens A vector of strings containing the command and its arguments.
 */
void changeDirectory(const std::vector<std::string>& tokens) {
    // Determine the target directory, defaulting to HOME if none is specified
    std::string dir = "";

    if(tokens.size() == 1){
        dir = getenv("HOME");
    }
    else {
        dir = tokens[1];
    }
    // Attempt to change to the specified directory
    int rc = chdir(dir.c_str());

    if (rc != 0) {
        // Report an error if changing directories fails
        perror("chdir");
    }
}


int main() {
    //Vector to keep track of background processes.
    std::vector<pid_t> backgroundProcesses;


    string input;

    //continuously read user input and execute commands until the user exits the shell
    while (true) {
        //Check if there is a child process
        checkBackgroundProcesses(backgroundProcesses);

        //Print the shell prompt to indicate that the shell is ready to accept input from the user
        cout << "> ";
        //cout<< &getDavPortrait;

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

        //Check and handle change-directory command
        if (tokens[0] == "cd") {
            changeDirectory(tokens);
            continue; // Skip further processing to prompt for the next command
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
            executeCommandProcess(command, backgroundProcesses);
        }

    }

    return 0;
}

std::string getDavPortrait(){
    return ";*?????*++*:+*::::::*+:;*?+:,:+;;;*?+++**;+*;:++**?%SS%?*+;;+*+*+??*?*+%+:::::+;+;;;+::::::++;;;:::;\n"
           "??*++;+*???***;;;;;*?+**+*+;++++++*?++***+++;;+?S#@#S???*++*?++**?*??*;?+:::::+;;;::;+:::::;;;;;;::+\n"
           "+++;+:::*;;+????*****?*;+*++*+++++**+%%**????%S#######S%%%%?%%******??;*+:::::+;;;:::+;::::;;;;;::;*\n"
           "++:;*::*+:::+*;+??%%S?????*******?%%S#SS##SSS#####@@@@@#@@@#S##%?**+++;*;:::::+;;;:::;+;:::;;:::::+;\n"
           "+::;*:++:;;:;*;*???%??????*+++**%S###@####SS#@#@@#@@@@@@@@@@@@##%*+;+;+?;:::::++;;::::;+:::::::,,:;,\n"
           "::;;*++::::;*??*++*?+;++****?%######@####S##@@@@@@@@@@@@@@@@@##S?*+++;:++:::::++;;:::::;:,,,,,,,,;;.\n"
           ":::;**:::;+*??++++**?++++*?S###@#@@@@@@####@@@@@@@@@@@@@@@@@@@@@#S%*++;*+:::::++;;:,,,,:;,,,,,..,;::\n"
           ":::;*;;;+??*+*++++?***+?S##@#S#@@@@@@@@@#@@@@@@@@@@@@@@@@@@@@@@@@@#S?+;++;:;;+++;;:,,,,,::.,,::;+++;\n"
           "::;**;*??*+++*++++?*+%S@@#@@S##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@##%*++++;;:;;;;:,,,,:;++++;;;++;;\n"
           ":;**?***+++++*++++?*?#@@#@@###@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#S?;,:;,,;;;++;+;+;;;+:;+++*???\n"
           "*????++++++;+*+++;*?#@####@#S#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#S?;;++++**++;;;;;+*++?%?+*??+\n"
           "?%%????**++;;*;+;;*S@#########@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#S*+;++;++++*?????**;+%+**?%?\n"
           ";????*********+;:;%@#########@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###S++*******???***++++*+%%S##\n"
           "+?*;***+;:;;+***+*#@#@######@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@###@%**+*************??**??**+\n"
           ";?+:+++**;::;+++*S#########@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#S##?++******+*********+;;;;;\n"
           "+**;++;;+*+++*+*S@@####@#@@@@@@@@@@@######################@@@@@@@@@@##@@#S##*+********+++++++++++++*\n"
           "+**+****+*****?#@@@@@####@@@@@@@@##SSS%%%%%%%%%%%%%%SSSSSS###@@@@@@@@@#@@#S#S+++++++++++++++++++++++\n"
           "+++++********%@@@@@@@####@@@@@@#SS%%%%%??????**??????%????%%S##@@@@@@@@###SS#*++++++++++++++++++++++\n"
           "%*****++++++*#@@@@@@@###@@@@@#SS%%???????***********?????????%SS#@@@@@@@###S#%++++++++++++++++++++++\n"
           "?+++++++++++?#@@@@@@####@@@@#%%%??????*********************???%%SS#@@@@@###S##++++++++++++++++++++++\n"
           "*+++***?++++%@@@@@@@###@@@@S%%????***************************???%%%#@@@@##SSS#+++++++++++++++++*****\n"
           "**??????*++?#@@@@@@####@@@#S%??*****************++***********??????%S@@@@#S###*+++++++*************+\n"
           "??%*??**+++?#@@@@@####@@@#S%?*****************++++++**+++++*****????%#@@@#S###?+***********+++++++++\n"
           "?+?*??***++*S#@@@@####@@#S%?******************+++++++++++++++*******?%##@#S###?+*+++++++++++++++++++\n"
           "?*????**+++?##@@######@##S%?****************++++++++++++++++++++******%S@@S##S*+++++++++++++++++++++\n"
           "*****++++++%@@@#####S@#SS%?**+**************+++++++++++++++++++++++****%@@#@S?++++++++++++++++++**??\n"
           "+++;;;;;;++S##########SSS%?*+++************+++++++++++++++++++++++++++*?#@##S*+++++++++++++?%%SSS###\n"
           "+++++++++++S#@#@#####SSSSS%?*++**********+++++++++**+++++++++++++++++++*%@##S++++++++++++++?########\n"
           "++++++++++*S########SSSSSS%*++++*********+++++++++++++++++++++++++++++++*S#S#*++**********%#########\n"
           "++++++++++*S########S#S#SS%*++++************+++++++++++++++++++++;++;++++?#SS?*******+++++S#########\n"
           "++++******?#########S##SSS?*+++++************+*+++++++++++++++++++;;;;;;;+##S%++++***++**+S######@#S\n"
           "*********+?########SS##SS%?*+++++***********++++++++++++++++++++++;;;;;;;;%#SS*****??+++++%#########\n"
           "**++++++++%########SSSSSS%?*+++++++*********++++++++++++++++++++++;;;;;;;;?#SS%###@@#+++++%#SS?????*\n"
           "++++++++++%#######SS#SSSS?*++++++++*****++**+++++++++++++++++++++;;;;;;;;;*#SSSS##@@#+++++*#@@###S**\n"
           "++++***?%%S######SS#SSSS%%*++++++++***++++++++++++++++++++++++++;;;;;;;;;;*SSS%S#####*++++*S@@@@S???\n"
           "%%SSS####SS#########SSSS%?+++++++*+++++++++++++++++++++++++++++;;;;;;;;;;+*SS%SS#####*++++*###@S????\n"
           "######SSS%#######SSSSSS%?+++++++***+++++++++++++++++++++++++++;;;;;;;;;;;+*%S%%%#####*++++?@#@@%????\n"
           "%%????%%%S#######SSSSS%?*+++++++*********++++++++*++++++++++++;++;;;;;;;;+*%#S%%#####*++++?##@S??%%%\n"
           "SS######SS#####S#SS#S%?*++++++++*???????****++++****+*+++++++++;+;;;;;;;;+*%SSS%#%S##*++++*###S?%%%%\n"
           "#######SS######S#SSSS?++++++++**?%%%%%%%%??***********+++*+++++++;+;;;;;;+*%SS%%S%%##*++++*#*%S%%%%%\n"
           "#######SS##########S%*+++++++**?%%%%%%%SSSS%????*******+*****+++++++++++;;+?SS?%??%##%**++*#%%%SS%%S\n"
           "SSSSSSSSS#########SS?++++++++*??%%%%%%%SSSSSS%%????*?**********????????*+;+?SS%%%?%SS##S*++S?S%S#%??\n"
           "SSSSSS%%S#######S##S?+++++++*????????%%%SSSSS%%%??????***??????%%SSSS%%%?*+?SS?%%?%S####S++++*?#S%?%\n"
           "S%%%%%%%S#######S##S*++++++*???????????%%%SSSS%%%%%??***??%%%%%%SS#SS%%%??+*SS?SS?S######*;+*?S@%??%\n"
           "%%%%%%??S###S#S##SS%*+++++*??????%%%%%%%SSSSSSS%%%%??***?%%%%%%SSSSS%%??%?*?#S?SSS#####@#*+;+?S#%%??\n"
           "%%?%%*+*?S####S##SS?+++++*?????%%%??????%%SSSSS%%%%??***?%%SSSSS%%??****????#S*%SSS#####%*;?**??????\n"
           "#S??*?%??%S###S##SS?+++++?????%%??%SSSSS%%?%SSS%%??***+*?%%SS%%%%%???****???#S*%SS*%###%*?SS+*??%???\n"
           "SSS*?SS%??%S####SS%*++++**???%%?%SS####SSSS%SSSS%?***+++*%%S%SSS%%%%%%%?**??#%*SS?+S###?%@@S+*?%%?%%\n"
           "?%S*%%??**?%SS##SS?+++++**????%SS??SS##S??SSSSSS%??*++++*%SSSS%%%??*+*%S?*??#%%SS**##@S?#@##**?SS%SS\n"
           "%%??%%?****%%SS#S%?++;+++***????%??####S??%%S%%%%??*++;;*%SSSSSS###S?*+?S???#?+%%+?##@S*@%*+*%%%%%SS\n"
           "S%*%%?*****??%SSS%*+;;+++*******???%%SS%??%%%%??%??*++;;+%%SSS####S#SS?*%???S?*%%+?###S??**+;+SSSSSS\n"
           "*??%%?*******%SSS?+;;;++++*******??????%%???%%????**++;;+?%SS%S####S%S#%??*?%**%?*%S##?????*+?SSSSSS\n"
           ";;?%??+******%SSS?+;;;++++********????????%%%%%???*++++;;*?%%%%%SS#%?%SS%?+*?+??*?%S#%???????SSSSSSS\n"
           "++?%?*****%%*?SS%*+;;;++++++*****????????%%%??******+++;;*?%%%%%%%????%%%?+*%*???%%%%?%%%%%%SSSSSSSS\n"
           "++?%?***?%S%**SS%++;;;++++++++***????????????*****+++++;;+*?%%%%%%%%%%?***+*%*?%?%%%%%%%%%%%SS#SSS#S\n"
           "*+*%?**??%S%**%S?+;;;++++++++++++*******?**?*******++++;;+***?????????*+++++?*%S?%%%%%%%SS%%S#######\n"
           "***??*??%S%*+*S%?+;+;;++++++++++++++***********++++++++;;;+++*????????*++++;?*%S%%%%%%%%SS%%S#######\n"
           "*?**?*?%%S?+;*%%*+++++;++++++++++++++********++++++++++;;;+++++*?????**++;;;**%#S%%S%%SSSS%%########\n"
           "%??*?*%%S%*+;+??*++++++++++++++++++++++++****+++++++++++;;;++++++*****+++;;;**%SS%SS%%SSSS%S###@####\n"
           "%%??*?%%%%*+;;+**+++++++++++++++++++++++******++++++++++;;;++++++++++;;;;;;;**%%%%SS%%%SSSSS###@####\n"
           ";*S?**%%%%?+;;++++*+++++++;;++++++++++++******++++++++++;;;;+++++++++;;;;;+;*?%%S%SS%%SSSSSS###@@###\n"
           ";;+%?*?????*;;+++***++++;;;;++++++++++****??**++++++++++;;;;+**++++++;;;;;;+*?%%S%SS%%SSSSSS###@@###\n"
           "+;;;****+*%*;;+++***+++;;;;;+++++++++***????*+++++++++++;;;++*?*++++;;;;;+++*?%S%SSS%%%SSS#S####@###\n"
           "++;;;*?*+*?*;;+++***++;;+;+++++++++*****????*++++++++++++++++*??**+++;;;;;;++?%S%SSS%%%%%S######@@@#\n"
           "**++;+*?++++;;+++***+++++++++++++******???%?*+++++++********+*???**++++;;;;+*?%S%SSS%%%%SS######@@@#\n"
           "%?*+++++++*+;;++++**++++++++++*******?????%?*********?********????**++++++++*?%S%SSSS##SSS######@@@@\n"
           "%%?*+++++**+;;+++++*+++++++*+******????????%??%%SS%??????*??**?????**+++++++??%S%S###SSS%SS#####@@@@\n"
           "S%%?**++***+;;+++++++++**********??????**??%%%%SSS?%%??%%S#S?**?????*****++*??%%SS%%%S%%%?*%S###@@@@\n"
           "%S%%?**+****;;++++++*+*********?????*****????%%??????%%%%SS%?***?????*****+++++++%%%S%%%?**????%#@@@\n"
           "?%%?********;;++++++*********?????***?**??????%%%%%%%%%%%%%?******????*****+++***%%%%%%%%???%???#@@@\n"
           "??*****??***+;+++++++***?**?????****????????????%%%S%%%%%%?******???????****?????%???????%??%???S#@@\n"
           "?*****?SS%?*+++++++++***???????****??????????????%%%%%%%????*****??*?????**+%%%%%%%%%%%?%?*?%??%S@@#\n"
           "?*****%#SSS%+++++++++**????*??****???????????????%%??????????**????**?%???*+%%%%%%SS%%%?%%?%%??%#@@#\n"
           "?***??%#SS#%+;++++++++*???**?***?????????????????????%%?????*?????????%???*+S#%%%%SSSS%%%S%%%??%@@@#\n"
           "??*???%#SSS%++++++++++***?**?***????%?%?????????????????????????%%????%%??*?##%%%%SSS##%%#%%%%%S@@##\n"
           "???????##S%S*+++++++++**??**?**??%%%%%%?????????????????*??????%%%%???%???%###S%%%%SSS#S%#SSS%S#@@@@\n"
           "???????%%%SS*++++*++++********???%%SS%%???????????????**??????%%%S%%%?%??%@@#SSSSS%%%%S#%S%?%%?S@@@#\n"
           "????????%SS#?+++++++++********???%%SSSS%%%%%%%%%%%%%???%%%%???%%SSS%%????%@#%?S###SSSS##S?%%??*S@@@@\n"
           "%%%????%SSS#%++++++*++*********???%%S##%%%%%??%??%%%%%%%%%%SSS#SSSS%%????S@S%?%#S%SSS%S@#%S%???S##@@\n"
           "%%%%???%#S##S+++++***+*********???%%%%S%??*?*+++++*******??%#@#S%%%%?????##%%S%SSSSSS%S#@##%???%##@@\n"
           "%%%%%???S*SS%++****+************???????%%??%?*?*++*++++++*?%#%??%%???????##SS%?SS%%%S%S#@@#S%??%#@@@\n"
           "%%%%%???**S?**+*****************??????**???????????***?????%%???????????%@#SS*+%S**?SS%#@@#%%??%#@@@\n"
           "%%%%%???**S?*+******************??????****************+****?????????????S@##%+++%SSSSS%#@##S%%?#@###\n"
           "%%%%????**S%*+******************?????************?*********??*??????????#@@#?++++###SS%#@SSS%%?S@@#@\n"
           "%%%%??????SS?++****************???????*********????***********?????????%@@#%*+++;%SSSS%##SSS%%?S#@@@\n"
           "%%%%??????S#%++****************???????*******??????????********????????S@@S?*++++?SSSS%##SSS%%%#####\n"
           "%%%%??????S#%++**********?******???????*****?????????????******???????%@@@#?**++++SSS%%#SSSS%%%#####\n"
           "%%%%????%?SS%++*****************???????*****?????????????******???????S@@@@?***+++?SS%SSS#SSS%%####@\n"
           "%%%%%??%%?SS%;+***?********?**??????????*****????????????*****???????%#@@@@S****++*SS%S###SS%%%##@@@\n"
           "%%%%%???%%SS%;+***???******?**??????????******???????????*****???????S@@@@@#?***+++%S%%###SS%%%SS##@\n"
           "%%%%%?%S#@#S%;++**????????*?**??????????*****????????*********??????%@@@@@@@%?***++*S%%####SS%%SSS##\n"
           "%%%%SS@@#@#S%;++**??????????*???????????*****?????????********??????#@@@@@@@S?***++*SS%####SS%%#####\n"
           "S##@@@@@###S?;++*???????????*????????????*********************?????%@@@@@@@@#??***+?SS%@####S%S#####\n"
           "S@@@@@@@#%?%?;++**????????????????????*??******************????????#@@@@@@@@@%?***+?SS%@#S#@S%S#####\n"
           "+?@@@@@@#%?%*;++***??????????????????**?*???*******?*******?*?????S@@@@@@@@@@@%%?**%S%%#SSS%S%S#####\n"
           "++?@@@@@#%??*;++***????%??????????***??*?????******??*****??*????%@@@@@@@@@@@@#SS%?%%%%#%?%%S%S#####\n"
           "+++%@@@@#%*?+;+++***???%%???????%??**???????******??*******?????%#@@@@@@@@@@@@#??%%%%%%S%???%SS####S\n"
           "++++#@@@#%?*;;+++***???%%%%%??????????*?????****???????*?*??????S@@@@@@@@@@@@@#?*????%%S%???%S######\n"
           "++++?@@@SSS++;+++****???%%%%%%%%%%?????????????????????????????%@@@@@@@@@@@@@@@%?***?S%##?**%S######\n"
           "+++++S@#SS#*++++++****??%%%%%%%%%%%???????????????%???????%???%#@@@@@@@@@@@@@@@??????%%%#??*%S######\n"
           "+++++?#SS##%*+++++****???%%%%%%%%%%%%%%%%%%%%????%%?%%%%%%%?%%#@@@ - PORTRAIT OF DAV   >";
}


