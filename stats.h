#ifndef __STATS_H__
#include <pthread.h>

typedef struct StatsManager
{
    int n_ack_threads;
    int n_threads;
    pthread_t *ack_threads;
    int *per_thread_static_requests_counter;
    int *per_thread_dynamic_requests_counter;
    pthread_mutex_t lock;

} sttmngr_t;

sttmngr_t *statManager;

void init_stat(int nthreads);
void init_array(int *arr, int size);
void acknowledge_thread();
int find_slot();
void write_header(char *hdr, char *buf);
void inc_static();
void destroy_stat();
#endif
