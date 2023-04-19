# System Programming Assigment 2

In this assigment we made 3 different tasks.

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

