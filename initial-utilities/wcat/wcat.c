#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 100

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        char buffer[BUF_LEN];
        while (fgets(buffer, BUF_LEN, fp) != NULL) {
            printf("%s", buffer);
        }
        fclose(fp);
    }
    return 0;
}
