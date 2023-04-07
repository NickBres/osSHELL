#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

void sigint_handler(int sig) {
    printf("\n");
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS];

    // set up signal handler to ignore SIGINT in the parent process
    signal(SIGINT, sigint_handler);


    while (1) {
        printf("stshell> ");
        fgets(command, MAX_COMMAND_LENGTH, stdin); // get input from user

        // remove newline character from command string
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

          // tokenize the command string into separate arguments
        int num_args = 0;
        char *arg = strtok(command, " ");
        while (arg != NULL && num_args < MAX_ARGUMENTS) {
            arguments[num_args] = arg;
            num_args++;
            arg = strtok(NULL, " ");
        }
        arguments[num_args] = NULL;

        int output_redirection = 0; // 0 = no redirection, 1 = >, 2 = >>
        int pipe_mode = 0; // 0 = no pipe, 1 = |
        char *output_file = NULL;
        int pipefd[2];

        // check for output redirection or pipe
        for (int i = 0; i < num_args; i++) {
            if (strcmp(arguments[i], ">") == 0) {
                output_redirection = 1;
                output_file = arguments[i + 1];
                arguments[i] = NULL;
                break;
            } else if (strcmp(arguments[i], ">>") == 0) {
                output_redirection = 2;
                output_file = arguments[i + 1];
                arguments[i] = NULL;
                break;
            } else if (strcmp(arguments[i], "|") == 0) {
                pipe_mode = 1;
                arguments[i] = NULL;
                break;
            }
        }

        // fork a new process to execute the command
        pid_t pid = fork();

        if (pid < 0) {
            printf("Failed to fork process.\n");
            exit(1);
        } else if (pid == 0) {
            // child process
            execvp(arguments[0], arguments);
            printf("Failed to execute command.\n");
            exit(1);
        } else {
            // parent process
            wait(NULL);
        }
    }

    return 0;
}
