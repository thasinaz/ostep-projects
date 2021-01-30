#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    size_t search_term_len = strlen(argv[1]);
    int i = 2;
    char *s = NULL;
    size_t n = 0;
    ssize_t line_len;
    do {
        FILE *fd = argc == 2 ? stdin : fopen(argv[i], "r");
        if (fd == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }
        while ((line_len = getline(&s, &n, fd)) != -1) {
            size_t match_len = 0;
            for (size_t j = 0; j < line_len; j++) {
                if (s[j] == argv[1][match_len]) {
                    match_len++;
                } else {
                    match_len = 0;
                }
                if (match_len == search_term_len) {
                    printf("%s", s);
                    break;
                }
            }
        }
        fclose(fd);
    } while (++i < argc);
    free(s);
    return 0;
}
