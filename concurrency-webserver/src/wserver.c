#include <stdio.h>
#include "io_helper.h"
#include "request.h"
#include "request_pool.h"
#include "worker.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int thread_num = 1;
    int buf_sz = 1;
    int schedalg = 0;
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
        switch (c) {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 't':
            thread_num = atoi(optarg);
            break;
        case 'b':
            buf_sz = atoi(optarg);
            break;
        case 's':
            if (!strcmp(optarg, "SFF")) {
                schedalg = 1;
            } else if (strcmp(optarg, "FIFO")) {
                fprintf(stderr, "schedalg must be one of FIFO or SFF\n");
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
            exit(1);
        }

    // run out of this directory
    chdir_or_die(root_dir);

    pool_init(buf_sz, schedalg);

    pthread_t tmp;
    for (int i = 0; i < thread_num; i++) {
        pthread_create_or_die(&tmp, NULL, work, NULL);
    }

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
        request_handle(conn_fd);
    }
    return 0;
}


    


 
