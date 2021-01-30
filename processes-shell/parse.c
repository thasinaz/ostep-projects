#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 20

// execute.h
extern int execute(char **args, char *direct);

char *trim_front(char *s) {
    if (s == NULL) {
        return NULL;
    }

    return s + strspn(s, " \t");
}

char **package_args(char s[]) {
    int i = 0;
    static char *args[MAX_ARGS];

    char *arg;
    do {
        s = trim_front(s);
        arg = strsep(&s, " \t");
        if (i >= MAX_ARGS) {
            return NULL;
        }
        args[i] = arg;
        i++;
    } while(arg != NULL && *arg != 0);
    args[i - 1] = NULL;
    return args;
}

int parse_command(char *command) {
    char *command_ptr = command;
    char *delim_ptr = NULL;
    char *file = NULL;
    int redirection = 0;
    while ((delim_ptr = strchr(command_ptr, '>')) != NULL) {
        if (redirection == 1) {
            return -1;
        }
        redirection = 1;

        *delim_ptr++ = 0;
        file = delim_ptr;
    }

    char **args = package_args(command);
    if (args == NULL) {
        return -1;
    }
    char *direct = trim_front(file);
    return execute(args, direct);
}

int parse_line(char *line) {
    char *line_ptr = line;
    char *delim_ptr;
    int n = 1;

    while ((delim_ptr = strchr(line_ptr, '&')) != NULL) {
        *delim_ptr = 0;
        if (parse_command(line_ptr) == -1) {
            return -1;
        }
        line_ptr = delim_ptr + 1;
        n++;
    }

    if (parse_command(line_ptr) == -1) {
        return -1;
    }

    int rc;
    while ((rc = wait(NULL)) != -1);

    if (errno != ECHILD) {
        return -1;
    }

    return 0;
}
