#include "io_helper.h"
#include "request.h"
#include "request_pool.h"

void *work(void *arg) {
    pthread_detach_or_die(pthread_self());
    while (1) {
        Request request = pool_get();
        if (request.is_satic) {
            request_serve_static(request.conn_fd, request.filename, request.file_sz);
        } else {
            request_serve_dynamic(request.conn_fd, request.filename, request.cgiargs);
        }
        close_or_die(request.conn_fd);
        free(request.filename);
        free(request.cgiargs);
    }
    return NULL;
}
