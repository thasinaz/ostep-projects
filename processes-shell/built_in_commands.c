#include <string.h>
#include <stdlib.h>
#include <unistd.h>
// env.h
extern int set_path(char *path);

int ext() {
    exit(0);
    return -2;
}

int cd(const char **path) {
    return chdir(path[1]);
}

int path(char **paths) {
    int PATH_len = 0;
    char *path;
    for (int i = 0; (path = paths[i]) != NULL; i++) {
        PATH_len += strlen(path) + 1;
    }

    char *new_path = (char *) malloc(PATH_len * sizeof(char));
    int index = 0;
    for (int i = 1; (path = paths[i]) != NULL; i++) {
        char ch;
        for (int j = 0; (ch = path[j]) != 0; j++) {
            new_path[index++] = ch;
        }
        new_path[index++] = ':';
    }
    new_path[index - 1] = 0;

    set_path(new_path);
    return 0;
}
