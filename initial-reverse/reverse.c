#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct Node{
    char *s;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
} List;

List new_List() {
    List L;
    L.head = NULL;
    return L;
}

void push(List *L, char *s) {
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    node->s = s;
    node->next = L->head;
    L->head = node;
}

char *pop(List *L) {
    if (L->head == NULL) {
        return NULL;
    }

    Node *top_item = L->head;
    char *s = top_item->s;
    L->head = top_item->next;
    free(top_item);
    return s;
}

int init(const int argc, char *argv[], FILE **input, FILE **output) {
    struct stat input_stat, output_stat;
    FILE *input_t = stdin, *output_t = stdout;
    input_t = stdin;
    output_t = stdout;
    switch (argc) {
        case 3:
            if (stat(argv[2], &output_stat) == -1) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
                exit(1);
            }
            if (stat(argv[1], &input_stat) == -1) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
                exit(1);
            }
            if (input_stat.st_ino == output_stat.st_ino && input_stat.st_dev == output_stat.st_dev) {
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
            output_t = fopen(argv[2], "w");
            if (output_t == NULL) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
                exit(1);
            }
        case 2:
            input_t = fopen(argv[1], "r");
            if (input_t == NULL) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
                exit(1);
            }
        case 1:
            *input = input_t;
            *output = output_t;
            return 1;
        default:
            return 0;
    }
}

void fread_lines(List *L, FILE *fd) {
    char *s = NULL;
    size_t n = 0;
    while (getline(&s, &n, fd) != -1) {
        push(L, s);
        s = NULL;
        n = 0;
    }
}

void fprint_lines(List *L, FILE *fd) {
    char *s;
    while ((s = pop(L)) != NULL) {
        fprintf(fd, "%s", s);
    }
}

int main(int argc, char *argv[]) {
    FILE *input, *output;
    if (!init(argc, argv, &input, &output)) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    List list = new_List();

    fread_lines(&list, input);
    fclose(input);

    fprint_lines(&list, output);
    fclose(output);

    return 0;
}
