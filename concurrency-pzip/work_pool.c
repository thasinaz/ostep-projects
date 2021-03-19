#include <semaphore.h>

#define MAX_BUF (1 << 20)

static sem_t empty;
static sem_t full;
static sem_t mutex;
static pthread_mutex_t rc_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t rc_cond = PTHREAD_COND_INITIALIZER;

char *pages[MAX_BUF];
size_t page_size[MAX_BUF];
FILE *rc[MAX_BUF];
int precent[MAX_BUF];
size_t head = 0;
size_t tail = 0;
size_t next_rc = 0;

size_t inc_m(size_t i, size_t m) {
    if (++i == m) {
        return 0;
    }
    return i;
}

void pool_init() {
    sem_init(&empty, 0, MAX_BUF);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
}

void pool_produce(char *page, size_t size) {
    sem_wait(&empty);
    sem_wait(&mutex);
    pages[tail] = page;
    page_size[tail] = size;
    tail = inc_m(tail, MAX_BUF);
    sem_post(&mutex);
    sem_post(&full);
}

char *pool_comsume(size_t *index) {
    if (head == tail) {
        return NULL;
    }
    sem_wait(&full);
    sem_wait(&mutex);
    char *rc = pages[head];
    *index = head;
    head = inc_m(head, MAX_BUF);
    sem_post(&mutex);
    sem_post(&empty);
    return rc;
}
