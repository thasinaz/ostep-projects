#include "request_pool.h"
#include "io_helper.h"

static Pool pool = { NULL, 0, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER };

void pool_init(int buf_sz, int schedalg) {
    if (pool.requests != NULL) {
        free(pool.requests);
    }
    pool.requests = (Request *) malloc_or_die(sizeof(Request) * (buf_sz + 1));
    pool.head = 0;
    pool.tail = 0;
    pool.max_size = buf_sz + 1;
    pool.schedalg = schedalg;
}

static size_t inc_m(size_t i, size_t m) {
    i++;
    if (i == m) {
        i = 0;
    }
    return i;
}

static size_t dec_m(size_t i, size_t m) {
    if (i == 0) {
        i = m;
    }
    i--;
    return i;
}

static int pool_empty() {
    return pool.head == pool.tail;
}

static int pool_full() {
    return pool.head == inc_m(pool.tail, pool.max_size);
}

void pool_put(Request *request) {
    pthread_mutex_lock_or_die(&pool.mutex);
    while (pool_full(pool)) {
        pthread_cond_wait_or_die(&pool.empty, &pool.mutex);
    }
    size_t pos = pool.tail;
    size_t next_tail = inc_m(pos, pool.max_size);

    size_t tmp = dec_m(pos, pool.max_size);
    while (pool.schedalg && pos != pool.head && request->file_sz < pool.requests[tmp].file_sz) {
        pool.requests[pos] = pool.requests[tmp];
        pos = tmp;
        tmp = dec_m(tmp, pool.max_size);
    }

    pool.requests[pos] = *request;
    pool.tail = next_tail;
    pthread_cond_signal_or_die(&pool.fill);
    pthread_mutex_unlock_or_die(&pool.mutex);
}

Request pool_get() {
    pthread_mutex_lock_or_die(&pool.mutex);
    while (pool_empty(pool)) {
        pthread_cond_wait_or_die(&pool.fill, &pool.mutex);
    }

    Request request = pool.requests[pool.head];
    pool.head = inc_m(pool.head, pool.max_size);

    pthread_cond_signal_or_die(&pool.empty);
    pthread_mutex_unlock_or_die(&pool.mutex);
    return request;
}
