#include "queue.h"
#include <stdlib.h>

queue_t *init(int queue_max_length)
{

    queue_t *this = malloc(sizeof(queue_t));
    if (this == NULL)
    {
        return this;
    }
    this->queue_max_length = queue_max_length;
    this->length = 0;
    this->next_cell = 0;
    this->oldest_record = -1;
    qnode_t *array = malloc(sizeof(qnode_t) * queue_max_length);
    if (array == NULL)
    {
        free(this);
        return NULL;
    }
    this->queue = array;

    if (pthread_mutex_init(&this->lock, NULL) < 0)
    {
        free(array);
        free(this);
        return NULL;
    }
    if ((pthread_cond_init(&this->full, NULL) < 0) || (pthread_cond_init(&this->empty,NULL) <0))
    {
        pthread_cond_destroy(&this->full);
        pthread_cond_destroy(&this->empty);
        pthread_mutex_destroy(&this->lock);
        free(array);
        free(this);
        return NULL;
    }
    return this;


}

void destroy(queue_t *q)
{
    if(q == NULL)
    {
        return;
    }
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->full);
    pthread_cond_destroy(&q->empty);
    free(q->queue);
    free(q);
}

int enqueue(queue_t *q, qnode_t node)
{
    if (q == NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&q->lock);
    while (q->length >= q->queue_max_length) // queue is full
    {
        pthread_cond_wait(&q->full,&q->lock);
    }
    q->queue[q->next_cell] = node;
    if (q->length == 0)
    {
        q->oldest_record = q->next_cell;
    }
    q->next_cell = (q->next_cell + 1) % q->queue_max_length;
    q->length++;
    pthread_cond_signal(&q->empty);
    pthread_mutex_unlock(&q->lock);
    return 0;
}
int dequeue(queue_t *q, qnode_t *node)
{

    if ((q == NULL) || (node == NULL))
    {
        return -1;
    }
    pthread_mutex_lock(&q->lock);
    while(q->length <= 0)
    {
        pthread_cond_wait(&q->empty,&q->lock);
    }
    q->length--;
    *node = q->queue[q->oldest_record];
    q->oldest_record = (q->oldest_record + 1) % q->queue_max_length;
    if (q->length == 0)
    {
        q->oldest_record = -1;
    }
    pthread_cond_signal(&q->full);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int count_free_cells(queue_t *q)
{
    if (q == NULL)
    {
        return -1;
    }
    return q->queue_max_length - q->length;
}