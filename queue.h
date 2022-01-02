#ifndef __QUEUE__
#include <pthread.h>



typedef struct QueueNode
{
    pthread_t thread_id;
    int connfd;
} qnode_t;

typedef struct Queue
{
    int queue_max_length;
    int length;
    int next_cell;
    int oldest_record;
    qnode_t *queue;
    mutex_t lock;
    cond_t cond;

} queue_t;

queue_t *init(int queue_max_length);
int enqueue(queue_t *q, qnode_t node);
int dequeue(queue_t *q, qnode_t *node);
int count_free_cells(queue_t *q);
void destroy(queue_t *q);

#endif
