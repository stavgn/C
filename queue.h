#ifndef __QUEUE__
#include <pthread.h>

typedef struct Queue
{
    int queue_max_length;
    int length;
    int next_cell;
    int oldest_record;
    int **queue;

} queue_t;

typedef struct QueueNode
{
    pthread_t thread_id;
    int connfd;
} qnode_t;

queue_t *init(int queue_max_length);
int enqueue(queue_t *q, qnode_t *node);
qnode_t *dequeue(queue_t *q);
int count_free_cells(queue_t *q);
void destroy(queue_t *q);

#endif
