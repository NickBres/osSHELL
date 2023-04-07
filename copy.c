#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

bool verbose = false;
bool force = false;

int copy_file(const char *src, const char *dst) {
    FILE *source, *destination;
    int ch;

    source = fopen(src, "rb");
    if (source == NULL) {
        return 1;
    }

    struct stat st;
    if (stat(dst, &st) == 0 && !force) {
        fclose(source);
        return 2;
    }

    destination = fopen(dst, "wb");
    if (destination == NULL) {
        fclose(source);
        return 1;
    }

    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: copy <source> <destination> [-v] [-f]\n");
        return 1;
    }

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-f") == 0) {
            force = true;
        } else {
            printf("Unknown flag: %s\n", argv[i]);
            return 1;
        }
    }

    int result = copy_file(argv[1], argv[2]);

    if (verbose) {
        switch (result) {
            case 0:
                printf("success\n");
                break;
            case 1:
                printf("general failure\n");
                break;
            case 2:
                printf("target file exists\n");
                break;
        }
    }

    return result;
}
