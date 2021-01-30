#include <stdlib.h>

char **get_path_address() {
    static char *PATH = NULL;
    return &PATH;
}

char *get_path() {
    return *get_path_address();
}

void set_path(char *path) {
    char **PATH = get_path_address();
    free(*PATH);
    *PATH = path;
}
