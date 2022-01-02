#include "segel.h"
#include "request.h"
#include "queue.h"
#include <pthread.h>
//
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too

queue_t *incoming_requests;
queue_t *handled_requests;

int get_num_of_available_reqs()
{
    return count_free_cells(incoming_requests) + count_free_cells(handled_requests);
}

void getargs(int *port, int *nthreads, int *queue_size, int *schedalg, int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, " port : %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *nthreads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    *schedalg = atoi(argv[4]);
}

typedef struct thread_args
{
    queue_t *incoming_requestes;
    queue_t *handled_requests;
} thread_args_t;

void *thread_worker()
{
    while (1)
    {
        qnode_t request;
        int err = dequeue(incoming_requests, &request);
        printf("%d", err);
        if (err == -1)
        {
            printf("No request found");
            continue;
        }
        request.thread_id = pthread_self();
        enqueue(handled_requests, request);
        requestHandle(request.connfd);
        Close(request.connfd);
        dequeue(handled_requests, &request);
    }
    return NULL;
}

void init_threads(int nthreads)
{
    while (nthreads--)
    {
        pthread_t t;
        pthread_create(&t, NULL, thread_worker, NULL);
    }
};

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, nthreads, queue_size, schedalg;
    struct sockaddr_in clientaddr;

    getargs(&port, &nthreads, &queue_size, &schedalg, argc, argv);

    incoming_requests = init(queue_size);
    handled_requests = init(queue_size);

    init_threads(nthreads);
    listenfd = Open_listenfd(port);
    while (1)
    {
        if (get_num_of_available_reqs() == 0)
        {
            pthread_cond_wait(&handled_requests->full, &handled_requests->lock);
        }
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        qnode_t *request = malloc(sizeof(qnode_t));
        request->connfd = connfd;
        int err = enqueue(incoming_requests, *request);
        if (err == -1)
        {
            //TODO
        }
    }
}
