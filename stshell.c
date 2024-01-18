#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define MAX_PROMPT 20

char prompt[MAX_PROMPT] = "hello: ";
int last_exit_status = 0;
char last_command[MAX_LINE];

typedef struct
{
    char *name;
    char *value;
} Variable, *VariablePtr;

Variable *variables = NULL; // dynamic array of variables
int var_count = 0;          // number of variables in the array

char *get_variable_value(char *name)
{
    for (int i = 0; i < var_count; i++)
    { // Loop through the variables
        if (strcmp(variables[i].name, name) == 0)
        { // Check if the name matches
            return variables[i].value;
        }
    }
    return NULL; // Return NULL if the variable was not found
}

void free_variable(char* varname){
    for(int i = 0; i < var_count; i++){
        if(strcmp(variables[i].name, varname) == 0){
            free(variables[i].name);
            free(variables[i].value);
            for(int j = i; j < var_count - 1; j++){
                variables[j] = variables[j + 1];
            }
            var_count--;
            variables = realloc(variables, var_count * sizeof(Variable));
            return;
        }
    }
}

void free_variables()
{
    for (int i = 0; i < var_count; i++)
    {
        free(variables[i].name);
        free(variables[i].value);
    }
    free(variables);
}

void set_variable(char *name, char *value)
{
    //check if the variable already exists
    if(get_variable_value(name) != NULL){
        // if it does, free the old value
        free_variable(name);
    }
    // realloc the array to make room for the new variable
    variables = realloc(variables, (var_count + 1) * sizeof(Variable));

    // Copy the name and value into the new array element
    variables[var_count].name = strdup(name);
    variables[var_count].value = strdup(value);

    // increment the variable count
    var_count++;
}

void handle_sigint(int sig)
{
    // Do nothing
    printf("You typed Control-C!\n");
}

char *remove_spaces(const char *str) {
    int len = strlen(str);
    char *no_spaces = malloc(len + 1); // +1 for the null-terminator
    char *current = no_spaces;

    for (int i = 0; i < len; i++) {
        if (!isspace((unsigned char)str[i])) {
            *current++ = str[i];
        }
    }

    *current = '\0'; // Null-terminate the new string
    return no_spaces;
}



void echo(char *args[])
{
    if(args[0] == NULL){
        return;
    }
    else if (strcmp(args[0], "$?") == 0) // Print the exit status
    {
        printf("%d\n", last_exit_status);
    }else if(args[0][0] == '$'){ // Print the value of a variable
        char* value = get_variable_value(args[0] + 1);
        if(value != NULL){
            printf("%s\n", value);
        }
    }
    else
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            printf("%s ", args[i]);
        }
        printf("\n");
    }
}

void file_riderect(int output_redirect, int error_redirect, char *output_file, char *error_file)
{
    // Redirect stdout to a file if necessary
    if (output_redirect > 0)
    {
        int fd = open(output_file, (output_redirect == 1) ? (O_WRONLY | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_APPEND), 0644);
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

    // Redirect stderr to a file if necessary
    if (error_redirect > 0)
    {
        int fd = open(error_file, (error_redirect == 1) ? (O_WRONLY | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_APPEND), 0644);
        if (fd == -1)
        {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDERR_FILENO) == -1)
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
    int error_redirect = 0;
    char *output_file = NULL;
    char *error_file = NULL;
    for (int j = 0; args[j] != NULL; j++)
    {
        if (strcmp(args[j], ">") == 0)
        {
            output_redirect = 1;
            output_file = args[j + 1];
            args[j] = NULL;
            break;
        }
        else if (strcmp(args[j], ">>") == 0)
        {
            output_redirect = 2;
            output_file = args[j + 1];
            args[j] = NULL;
            break;
        }
        else if (strcmp(args[j], "<") == 0)
        {
            int fd = open(args[j + 1], O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }
            if (dup2(fd, STDIN_FILENO) == -1)
            {
                perror("dup2");
                exit(1);
            }
            close(fd);
            args[j] = NULL;
            break;
        }
        else if (strcmp(args[j], "2>") == 0)
        {
            error_redirect = 1;
            error_file = args[j + 1];
            args[j] = NULL;
            break;
        }
        else if (strcmp(args[j], "2>>") == 0)
        {
            error_redirect = 2;
            error_file = args[j + 1];
            args[j] = NULL;
            break;
        }
    }
    file_riderect(output_redirect, error_redirect, output_file, error_file);

    // Execute the command
    if (execvp(args[0], args) == -1)
    {
        perror("execvp");
        exit(1);
    }
}

int main()
{
    // Set up signal handling for Ctrl+c
    signal(SIGINT, handle_sigint);

    while (1)
    {
        char input[MAX_LINE + 1]; // +1 for the null terminator

        // Display the prompt and read the user's command
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);

        // Remove the newline character from the end of the input
        input[strcspn(input, "\n")] = 0;

        // Exit if the user enters "exit"
        if (strcmp(input, "quit") == 0)
        {
            free_variables();  // Free the variables array
            exit(0);
        }

        // Check for !! command
        if (strcmp(input, "!!") == 0)
        {
            if (strlen(last_command) == 0)
            {
                printf("No commands in history.\n");
                continue;
            }
            strcpy(input, last_command);
        }
        else
        { // Save the command in history
            strcpy(last_command, input);
        }

        // Check for cd command
        if (strncmp(input, "cd ", 3) == 0)
        {
            char *dir = input + 3; // Get the directory name
            if (chdir(dir) == -1)
            {
                perror("chdir");
            }
            continue;
        }

        // Check if input contains a prompt change
        char newPrompt[MAX_PROMPT];
        if (sscanf(input, "prompt = %s", newPrompt) == 1)
        {
            strcpy(prompt, newPrompt);
            strcat(prompt, " "); // add a space after the prompt
            continue;
        }

        // check for echo command
        if (strncmp(input, "echo ", 5) == 0)
        {
            // Tokenize the rest of the input and pass it to echo
            char *echo_args[256];
            int count = 0;
            char *token = strtok(input + 5, " ");
            while (token != NULL)
            {
                echo_args[count++] = token;
                token = strtok(NULL, " ");
            }
            echo_args[count] = NULL; // Terminate the array with a NULL pointer
            echo(echo_args);
            continue;
        }

        // check for variable assignment
        if (input[0] == '$')
        {
            char *var_name = strtok(input + 1, "="); // Get the variable name
            if (var_name != NULL)
            {
                char *var_value = strtok(NULL, ""); // Get the rest of the input as the variable value
                if (var_value != NULL)
                {
                    char *clear_value = remove_spaces(var_value);
                    char *clear_name = remove_spaces(var_name);

                    // create or update the variable
                    set_variable(clear_name, clear_value);
                    free(clear_name);
                    free(clear_value);
                }
            }
            continue;
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
            int pipes[pipe_count][2]; // 2 file descriptors per pipe 0: input, 1: output
            for (int i = 0; i < pipe_count; i++)
            {
                if (pipe(pipes[i]) == -1) // create a pipe
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
                if (WIFEXITED(status))
                {
                    last_exit_status = WEXITSTATUS(status); // Capture exit status
                }
            }
        }
    }
    return 0;
}
