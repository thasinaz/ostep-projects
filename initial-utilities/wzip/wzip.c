#include <stdio.h>
#include <stdlib.h>

#define BUF_LEN 100

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    char buf[BUF_LEN];
    int acc = 0;
    char ch;
    for (int i = 1; i < argc; i++) {
        FILE *fd = fopen(argv[i], "r");
        if (fd == NULL) {
            printf("wzip: cannot open file\n");
            exit(1);
        }
        while (fgets(buf, BUF_LEN, fd) != NULL) {
            int index = 0;
            if (!acc) {
                ch = buf [0];
                acc = 1;
                index = 1;
            }
            while (1) {
                char tmp = buf[index];
                if (tmp == 0) {
                    break;
                } else if (tmp == ch) {
                    acc++;
                } else {
                    fwrite(&acc, 4, 1, stdout);
                    printf("%c", ch);
                    acc = 1;
                    ch = tmp;
                }
                index++;
            }
        }
        fclose(fd);
    }
    fwrite(&acc, 4, 1, stdout);
    printf("%c", ch);
    return 0;
}
