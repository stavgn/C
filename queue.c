

#include "queue.h"

queue_t *init(int queue_max_length);
int enqueue(queue_t *q, int fd);
int dequeue(queue_t *q);
int count_free_cells(queue_t *q);
void destroy(queue_t *q);