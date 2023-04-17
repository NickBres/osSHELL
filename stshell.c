#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

void handle_sigint(int sig)
{
    // Do nothing
    printf("\n");
}

void file_riderect(char *input, int output_redirect, char *output_file)
{
    // Redirect output to a file if necessary
    if (output_redirect == 1)
    {
        // Output redirection with ">"
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd);
    }
    else if (output_redirect == 2)
    {
        // Output redirection with ">>"
        int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd);
    }
}

void execute(char *input)
{
    char *args[256];
    char *arg = strtok(input, " ");
    int i = 0;
    while (arg != NULL)
    {
        args[i++] = arg;
        arg = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Check if this command contains an output redirection symbol
    int output_redirect = 0;
    char *output_file = NULL;
    for (int j = 0; args[j] != NULL; j++)
    {
        if (strcmp(args[j], ">") == 0)
        {
            output_redirect = 1;
            output_file = args[j + 1];
            break;
        }
        else if (strcmp(args[j], ">>") == 0)
        {
            output_redirect = 2;
            output_file = args[j + 1];
            break;
        }
    }
    file_riderect(input, output_redirect, output_file);

    // Execute the command
    if (execvp(args[0], args) == -1)
    {
        perror("execvp");
        exit(1);
    }
}

int main()
{
    while (1)
    {
        char input[MAX_LINE + 1]; // +1 for the null terminator

        // Display the prompt and read the user's command
        printf("stshell> ");
        fgets(input, sizeof(input), stdin);

        // Remove the newline character from the end of the input
        input[strcspn(input, "\n")] = 0;

        // Exit if the user enters "exit"
        if (strcmp(input, "exit") == 0)
        {
            exit(0);
        }

        // Check if the input contains a pipe
        int pipe_count = 0;
        for (int i = 0; i < strlen(input); i++)
        {
            if (input[i] == '|')
            {
                pipe_count++;
            }
        }

        if (pipe_count > 0)
        {
            // Split the input by pipes and store each command in an array
            char *commands[pipe_count + 1];
            char *token = strtok(input, "|");
            int i = 0;
            while (token != NULL)
            {
                commands[i++] = token;
                token = strtok(NULL, "|");
            }
            commands[i] = NULL;

            // Create an array of pipes for each command
            int pipes[pipe_count][2];
            for (int i = 0; i < pipe_count; i++)
            {
                if (pipe(pipes[i]) == -1)
                {
                    perror("pipe");
                    exit(1);
                }
            }

            // Fork a child process for each command in the pipeline
            int pid;
            for (int i = 0; i < pipe_count + 1; i++)
            {
                pid = fork();

                if (pid == -1)
                {
                    perror("fork");
                    exit(1);
                }
                else if (pid == 0)
                {
                    // Child process

                    // Set up signal handling for Ctrl+c
                    signal(SIGINT, handle_sigint);

                    // Redirect input from the previous pipe, if this is not the first command
                    if (i > 0)
                    {
                        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) // duplciate the read end of the previous pipe to the standard input
                        {
                            perror("dup2");
                            exit(1);
                        }
                    }

                    // Redirect output to the next pipe, if this is not the last command
                    if (i < pipe_count)
                    {
                        if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            exit(1);
                        }
                    }

                    // Close all pipe ends except the ones being used by this command
                    for (int j = 0; j < pipe_count; j++)
                    {
                        if (i > 0 && j == i - 1)
                        {
                            close(pipes[j][1]);
                        }
                        else if (i < pipe_count && j == i)
                        {
                            close(pipes[j][0]);
                        }
                        else
                        {
                            close(pipes[j][0]);
                            close(pipes[j][1]);
                        }
                    }
                    execute(commands[i]);
                }
            }

            // Close all pipe ends in the parent process
            for (int i = 0; i < pipe_count; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            // Wait for all child processes to exit
            int status;
            for (int i = 0; i < pipe_count + 1; i++)
            {
                wait(&status);
            }
        }
        else
        {
            // Execute the command normally
            int pid = fork();

            if (pid == -1)
            {
                perror("fork");
                exit(1);
            }
            else if (pid == 0)
            {
                // Child process

                // Set up signal handling for Ctrl+c
                signal(SIGINT, handle_sigint);
                // Execute the command
                execute(input);
            }
            else
            {
                // Parent process

                // Wait for the child process to exit
                int status;
                wait(&status);
            }
        }
    }
    return 0;
}
