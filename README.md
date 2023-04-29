# System Programming Assigment 2

In this assigment we made 3 different tasks.

All tests has been made on MacBook Pro with arm proccesor using UTM virtual machine to run gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0.


## TaskA: Compare and Copy files tool

### Compare

Cmp tool compares two files and returns 1 if they are equal or 0 if not. The tool supports additional flags (-v , -i). -v for verbose output. -i to ignore case size.

To run:
```sh
   make cmp
   ./cmp <file1> <file2> -v -i
   ```
   
### Copy

Copy tool will copy a source file to a destination file. The tool supports additional flags (-v, -f). -v for verbose output. -f will force copy (will rewrite existing file if needed).

To run:
```sh
   make copy
   ./copy <file1> <file2> -v -f
   ```

## TaskB: Dynamic libraries implementing

In this taks we wrote two simple dynamic libraries that converts characters they get. Also we wrote two simple tools that demnstrate dynamic library usage.

Libraries:
   CodecA - makes swaps every big char to small and small to big.
   CodecB - adds 3 to ascii of every char.
   
 Tools:
   Encode - gets codec name and text, prints encoded text to the console
   Decode - gets codec name and text, prints decodedd text to the console
   
To run:
   
```sh
   make codecA codecB encode decode
   export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
   ./encode <codec> "some text"
   ./decode <codec> "some text"
   ```
## TaskA and TaskB Example

<img width="639" alt="image" src="https://user-images.githubusercontent.com/70432147/233020933-62c795f4-0418-4a64-9f8f-b9877d7c7ea3.png">


## TaskC: Shell Programm

This is a C program that implements a simple shell, which allows users to execute commands by typing them into the terminal. The shell supports basic features such as input/output redirection and piping.

Here is a brief overview of the program's functionality:

* The program defines several functions to handle signal handling, input/output redirection, and command execution.
* The main function enters an infinite loop, displaying a prompt and reading user input until the user types "exit".
* If the user input contains a pipe character ("|"), the program splits the input into multiple commands and creates a pipeline of child processes to execute the commands.
* If the user input does not contain a pipe character, the program simply forks a child process to execute the command.
* In both cases, the child process sets up signal handling for Ctrl+c, redirects input/output if necessary, and executes the command using the execvp() function.
* The parent process waits for the child process to exit and then repeats the loop.

To run:

```sh
   make stshell
   ./stshell
   ```
## TaskC Example


  <img width="1126" alt="image" src="https://user-images.githubusercontent.com/70432147/233023162-f9e438e5-50ca-4441-906a-f82166ffe42f.png">

