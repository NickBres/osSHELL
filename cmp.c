#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define BUFFER_SIZE 4096

bool verbose = false;
bool ignore_case = false;

int compare_files(FILE *file1, FILE *file2) {
    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];
    size_t read1, read2;

    while ((read1 = fread(buffer1, 1, BUFFER_SIZE, file1)) > 0 &&
           (read2 = fread(buffer2, 1, BUFFER_SIZE, file2)) > 0) {

        if (ignore_case) {
            for (size_t i = 0; i < read1; i++) {
                buffer1[i] = tolower(buffer1[i]);
                buffer2[i] = tolower(buffer2[i]);
            }
        }

        if (read1 != read2 || memcmp(buffer1, buffer2, read1) != 0) {
            return 1;
        }
    }

    return (feof(file1) && feof(file2)) ? 0 : 1;
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: cmp <file1> <file2> [-v] [-i]\n");
        return 1;
    }

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-i") == 0) {
            ignore_case = true;
        } else {
            printf("Unknown flag: %s\n", argv[i]);
            return 1;
        }
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");

    if (!file1 || !file2) {
        printf("Error opening files.\n");
        return 1;
    }

    int result = compare_files(file1, file2);

    if (verbose) {
        printf("%s\n", (result == 0) ? "equal" : "distinct");
    }

    fclose(file1);
    fclose(file2);

    return result;
};
