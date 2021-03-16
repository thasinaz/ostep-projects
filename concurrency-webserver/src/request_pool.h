#ifndef __REQUEST_POOL_H__
#include <pthread.h>

#define MAXBUF (8192)

typedef struct __Request {
    int conn_fd;
    int is_satic;
    char *filename;
    char *cgiargs;
    int file_sz;
} Request;

typedef struct __Pool {
    Request *requests;
    size_t head, tail;
    size_t max_size;
    int schedalg;
    pthread_mutex_t mutex;
    pthread_cond_t empty, fill;
} Pool;

void pool_init(int buf_sz, int schedalg);
void pool_put(Request *request);
Request pool_get();

#endif // __REQUEST_POOL_H__
