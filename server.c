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

void *thread_worker(void *param)
{
    while (1)
    {
        thread_args_t args = *((thread_args_t *)param);
        qnode_t request;
        int err = dequeue(args.incoming_requestes, &request);
        if (err == -1)
        {
            break;
        }
        request.thread_id = pthread_self();
        enqueue(args.handled_requests, request);
        requestHandle(request.connfd);
        Close(request.connfd);
        dequeue(args.handled_requests, &request);
    }
    return NULL;
}

void init_threads(int nthreads, queue_t *q1, queue_t *q2)
{
    thread_args_t args;
    args.incoming_requestes = q1;
    args.handled_requests = q2;

    while (nthreads--)
    {
        pthread_t t;
        pthread_create(&t, NULL, &thread_worker, (void *)&args);
    }
};

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, nthreads, queue_size, schedalg;
    struct sockaddr_in clientaddr;

    getargs(&port, &nthreads, &queue_size, &schedalg, argc, argv);

    queue_t *waiting_requests = init(queue_size);
    queue_t *handled_requests = init(queue_size);

    init_threads(nthreads, waiting_requests, handled_requests);
    listenfd = Open_listenfd(port);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        qnode_t request;
        request.connfd = connfd;
        int err = enqueue(waiting_requests, request);
        if (err == -1)
        {
            //TODO
        }
    }
}
