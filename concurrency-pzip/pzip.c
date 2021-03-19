#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <fcntl.h>

#include "work_pool.c"

#define BUF_LEN 1024
#define MAX_THREAD 128

static int cnt = 0;
static char ch = 0;
static int begin = 1;

void *Malloc(size_t size) {
    void *rc = malloc(size);
    assert(rc != NULL);
    return rc;
}

void *work(void *arg) {
    while (1) {
        size_t index;
        char *page = pool_comsume(&index);
        if (page == NULL) {
            return NULL;
        }
        long size = page_size[index];
        FILE *fd = fmemopen(NULL, 5 * size + 4, "w+");
        int acc = 1;
        char ch = page[0];
        for (int i = 1; i < size; i++) {
            char tmp = page[i];
            if (tmp == ch) {
                acc++;
            } else {
                //printf("%d %c\n", acc, ch);
                fwrite(&acc, 4, 1, fd);
                fwrite(&ch, 1, 1, fd);
                acc = 1;
                ch = tmp;
            }
        }
        //printf("%d %c\n", acc, ch);
        fwrite(&acc, 4, 1, fd);
        fwrite(&ch, 1, 1, fd);
        acc = 0;
        //printf("%d %c\n", acc, ch);
        fwrite(&acc, 4, 1, fd);
        fflush(fd);
        rc[index] = fd;
    }
}

int data_out() {
    while (next_rc != tail) {
        fflush(stdout);
        int new_page = 1;
        FILE *fd = rc[next_rc];
        rewind(fd);
        while (1) {
            int tmp_cnt;
            char tmp_ch;
            fread(&tmp_cnt, 4, 1, fd);
            if (tmp_cnt == 0) {
                new_page = 1;
                break;
            }
            fread(&tmp_ch, 1, 1, fd);
            if (begin) {
                begin = 0;
                cnt = tmp_cnt;
                ch = tmp_ch;
                continue;
            }

            if (new_page && ch == tmp_ch) {
                new_page = 0;
                cnt += tmp_cnt;
                continue;
            }

            fwrite(&cnt, 4, 1, stdout);
            fwrite(&ch, 1, 1, stdout);
            ch = tmp_ch;
            cnt = tmp_cnt;
        }
        next_rc = inc_m(next_rc, MAX_BUF);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("pzip: file1 [file2 ...]\n");
        exit(1);
    }
    long _page_size = sysconf(_SC_PAGE_SIZE);
    pool_init();

    pthread_t thread[MAX_THREAD];
    int nprocs = get_nprocs();

    for (int i = 1; i < argc; i++) {
        struct stat file_stat;
        assert(stat(argv[i], &file_stat) == 0);
        int fd = open(argv[i], O_RDONLY);
        assert(fd != -1);
        off_t size = file_stat.st_size;
        for (int i = 0; i < size;) {
            for (int j = 0; i < size && j < MAX_BUF; j++) {
                size_t current_size;
                if (i + _page_size < size) {
                    current_size = _page_size;
                } else {
                    current_size = size - i;
                }
                char *page = (char *) mmap(NULL, current_size, PROT_READ, MAP_PRIVATE, fd, i);
                assert(page != (void *) -1);
                pool_produce(page, current_size);
                i += _page_size;
            }
            for (int i = 0; i < nprocs; i++) {
                assert(pthread_create(&thread[i], NULL, work, NULL) == 0);
            }
            for (int i = 0; i < nprocs; i++) {
                assert(pthread_join(thread[i], NULL) == 0);
            }
            data_out();
        }
    }

    if (cnt != 0) {
        fwrite(&cnt, 4, 1, stdout);
        fwrite(&ch, 1, 1, stdout);
    }
    return 0;
}
