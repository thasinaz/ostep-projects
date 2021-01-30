#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        FILE *fd = fopen(argv[i], "r");
        if (fd == NULL) {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        int n = 0;
        char ch;
        while (fread(&n, 4, 1, fd)) {
            fread(&ch, 1, 1, fd);
            while (n--){
                putchar(ch);
            }
        }
        fclose(fd);
    }
    return 0;
}
