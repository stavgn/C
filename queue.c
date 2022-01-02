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
    return this;


}

void destroy(queue_t *q)
{
    free(q->queue);
    free(q);
}

int enqueue(queue_t *q, qnode_t node)
{
    if (q->length == q->queue_max_length) // queue is full
    {
        return -1;
    }
    q->queue[q->next_cell] = node;
    if (q->length == 0)
    {
        q->oldest_record = q->next_cell;
    }
    q->next_cell = (q->next_cell + 1) % q->queue_max_length;
    q->length++;
    return 0;
}
int dequeue(queue_t *q, qnode_t *node)
{
    if ((node == NULL) || (q->length == 0))
    {
        return -1;
    }
    q->length--;
    *node = q->queue[q->oldest_record];
    q->oldest_record = (q->oldest_record + 1) % q->queue_max_length;
    if (q->length == 0)
    {
        q->oldest_record = -1;
    }
    return 0;
}

int count_free_cells(queue_t *q)
{
    return q->queue_max_length - q->length;
}