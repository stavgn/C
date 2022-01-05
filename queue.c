#include "queue.h"
#include <stdlib.h>

queue_t *init(int queue_max_length)
{

    queue_t *this = malloc(sizeof(queue_t));
    if (this == NULL)
    {
        return NULL;
    }
    this->queue_max_length = queue_max_length;
    this->length = 0;
    this->next_cell = 0;
    this->oldest_record = -1;
    this->working_threds = 0;
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
    if ((pthread_cond_init(&this->full, NULL) < 0) || (pthread_cond_init(&this->empty, NULL) < 0))
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
    if (q == NULL)
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
    while ((q->length + q->working_threds) >= q->queue_max_length) // queue is full
    {
        pthread_cond_wait(&q->full, &q->lock);
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
    while (q->length <= 0)
    {
        pthread_cond_wait(&q->empty, &q->lock);
    }
    q->length--;
    *node = q->queue[q->oldest_record];
    q->oldest_record = (q->oldest_record + 1) % q->queue_max_length;
    if (q->length == 0)
    {
        q->oldest_record = -1;
    }
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int handle(queue_t *q, qnode_t *node)
{
    if ((q == NULL) || (node == NULL))
    {
        return -1;
    }
    pthread_mutex_lock(&q->lock);
    if (dequeue(q, node) == 0)
    {
        q->working_threds++;
        pthread_mutex_unlock(&q->lock);
        return 0;
    }
    else
    {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }
}

int done(queue_t *q)
{
    if (q == NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&q->lock);
    q->working_threds--;
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

int *_random_sub_set(int range)
{
    int *array = malloc(sizeof(int) * range);
    if (array == NULL)
    {
        return NULL;
    }
    int *sorted_array = malloc(sizeof(int) * range);
    if (sorted_array == NULL)
    {
        free(array);
        return NULL;
    }
    for (int i = 0; i < range; i++)
    {
        array[i] = i;
        sorted_array[i] = 0;
    }

    srand(time(NULL));
    int r;
    for (int len = (range - 1); len > 0; len--) // shufel array
    {
        r = rand() % (len + 1);
        int tmp = array[r];
        array[r] = array[len];
        array[len] = tmp;
    }

    // takes the first range/2 random indexs

    for (int i = 0; i < range / 2 + (range % 2); i++)
    {
        sorted_array[array[i]] = 1;
    }
    free(array);
    return sorted_array;
}