#include "codecA.h"
#include "codecB.h"
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    system("export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH"); // to load the shared libraries
    if (argc != 3) {
        printf("Usage: encode <codec> <string>\n");
        return 1;
    }
    if (strcmp(argv[1], "codecA") == 0) {
        codecA_encode(argv[2]);
    } else if (strcmp(argv[1], "codecB") == 0) {
        codecB_encode(argv[2]);
    } else {
        printf("Unknown codec: %s, use codecA or codecB", argv[1]);
        return 1;
    };
    printf("%s\n", argv[2]);
};