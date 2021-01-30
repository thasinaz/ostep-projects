#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// built_in_commands.h
extern int ext();
extern int cd(char **path);
extern int path(char **paths);

// env.c
extern char *get_path();

// parse.c
extern char *trim_front(char *s);

int check_direct(char *direct) {
    strsep(&direct, " \t");
    if (direct != NULL && *trim_front(direct) != 0){
        return -1;
    }
    return 0;
}

char *find_path(char *file) {
    if (strncmp(file, "/", 1) == 0 || strncmp(file, "./", 2) == 0) {
        int rc = access(file, X_OK);
        if (rc == 0) {
            return file;
        }
        return NULL;
    }

    char *path = get_path();
    if (path == NULL) {
        return NULL;
    }
    static char *buf = NULL;
    static size_t n = 0;
    if (buf == NULL) {
        buf = malloc(128 * sizeof(char));
        n = 128;
    }

    size_t file_len = strlen(file);
    int rc;
    int realloc = 0;
    char *delim_ptr = NULL;
    do {
        delim_ptr = strchr(path, ':');
        size_t path_len = delim_ptr - path;
        if (delim_ptr == NULL) {
            path_len = strlen(path);
        }

        int complete = 0;
        if (path[path_len - 1] != '/') {
            complete = 1;
        }
        while (n <= complete + file_len + path_len) {
            n <<= 1;
            realloc = 1;
        }
        if (realloc) {
            free(buf);
            buf = malloc(n * sizeof(char));
            realloc = 0;
        }

        memcpy(buf, path, path_len);
        buf[path_len] = '/';
        memcpy(buf + path_len + complete, file, file_len + 1);

        rc = access(buf, X_OK);
        if (rc == 0) {
            return buf;
        }

        path = delim_ptr + 1;
    } while (delim_ptr != NULL);

    return NULL;
}

int is_built_in(char **args) {
    char *command = args[0];
    
    if (strcmp(command, "exit") == 0) {
        if (args[1] != NULL) {
            return -2;
        }
        return ext();
    } else if (strcmp(command, "cd") == 0) {
        if (args[2] != NULL) {
            return -2;
        }
        return cd(args);
    } else if (strcmp(command, "path") == 0) {
        return path(args);
    }
    return -1;
}

int execute(char **args, char *direct) {
    int redirection = 0;
    if (direct != NULL) {
        if (args[0] == NULL || check_direct(direct) == -1) {
            return -1;
        }
        redirection = 1;
    }

    if (args[0] == NULL) {
        return 0;
    }

    int built_in;
    if ((built_in = is_built_in(args)) == -2) {
        return -1;
    } else if (built_in == 0) {
        return 0;
    }

    char *path;
    if ((path = find_path(args[0])) == NULL) {
        return -1;
    }
    
    pid_t rc = fork();
    if (rc == -1) {
        return -1;
    }
    if (rc == 0) {
        if (redirection) {
            if (close(STDOUT_FILENO) == -1) {
                return -1;
            }

            if (open(direct, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU) == -1) {
                return -1;
            };

        }
        execv(path, args);
        return -1;
    }

    return 0;
}
