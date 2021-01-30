#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

// parse.h
extern int parse_line(char *line);

// env.h
extern void set_path(char *path);

void init() {
    set_path(strdup("/bin"));
}

void error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int main(int argc, char *argv[]) {

    init();

    // interactive mode or batch mode
    FILE *fd;
    int mode;
    if (argc == 1) {
        fd = stdin;
        mode = 1;
    } else if (argc == 2) {
        fd = fopen(argv[1], "r");
        if (fd == NULL) {
            error();
            exit(1);
        }
        mode = 0;
    } else {
        error();
        exit(1);
    }

    // waiting user to type the cammand
    char *line = NULL;
    size_t len = 0;
    int flag = 0;
    while (1) {
        if (mode) {
            printf("wish> ");
        }
        int line_len = getline(&line, &len, fd);
        if (line_len == -1) {
            if (mode == 0 && flag == 0) {
                exit(1);
            }
            exit(0);
        }
        if (line[line_len - 1] == '\n') {
            line[line_len - 1] = 0;
        };

        // prase
        int rc = parse_line(line);
        if (rc != 0) {
            error();
        } else {
            flag = 1;
        }
    }
    exit(0);
}
