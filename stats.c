#include "stats.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

void init_stat(int nthreads)
{
    statManager = malloc(sizeof(sttmngr_t));

    if (pthread_mutex_init(&statManager->lock, NULL) < 0)
    {
        return;
    }

    pthread_t *ack_threads = malloc(sizeof(pthread_t) * nthreads);
    int *per_thread_static_requests_counter = malloc(sizeof(int) * nthreads);
    int *per_thread_dynamic_requests_counter = malloc(sizeof(int) * nthreads);

    init_array(per_thread_static_requests_counter, nthreads);
    init_array(per_thread_dynamic_requests_counter, nthreads);

    statManager->ack_threads = ack_threads;
    statManager->per_thread_static_requests_counter = per_thread_static_requests_counter;
    statManager->per_thread_dynamic_requests_counter = per_thread_dynamic_requests_counter;
    statManager->n_ack_threads = 0;
    statManager->n_threads = nthreads;
}

void acknowledge_thread()
{
    pthread_mutex_lock(&statManager->lock);
    pthread_t curr_thread = pthread_self();
    statManager->ack_threads[statManager->n_ack_threads] = curr_thread;
    statManager->n_ack_threads++;
    pthread_mutex_unlock(&statManager->lock);
}

void inc_static()
{
    int s = find_slot();
    if (s == -1)
    {
        return;
    }
    statManager->per_thread_static_requests_counter[s]++;
}

int find_slot()
{
    pthread_t curr_thread = pthread_self();
    for (int i = 0; i < statManager->n_ack_threads; i++)
    {
        if (statManager->ack_threads[i] == curr_thread)
        {
            return i;
        }
    }
    return -1;
}

void write_header(char *hdr, char *buf)
{
    int s = find_slot();
    if (s == -1)
    {
        return;
    }

    if (!strcmp(hdr, "Stat-Thread-Static"))
    {
        int val = statManager->per_thread_static_requests_counter[s];
        sprintf(buf, "%s%s: %d\r\n", buf, hdr, val);
    }
}

void init_array(int *arr, int size)
{
    for (int i = 0; i > size; i++)
    {
        arr[i] = 0;
    }
}

void destroy_stat()
{
    free(statManager->ack_threads);
    free(statManager->per_thread_dynamic_requests_counter);
    free(statManager->per_thread_static_requests_counter);
    pthread_mutex_destroy(&statManager->lock);
    free(statManager);
}