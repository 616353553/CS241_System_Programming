/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include "barrier.h"

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    int error = pthread_mutex_destroy(&(barrier->mtx));
    if (error != 0)
        return error;
    return pthread_cond_destroy(&(barrier->cv));
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    int error = pthread_mutex_init(&(barrier->mtx), NULL);
    if (error != 0)
        return error;
    error = pthread_cond_init(&(barrier->cv), NULL);
    if (error != 0)
        return error;
    barrier->n_threads = num_threads;
    barrier->count = 0;
    barrier->times_used = 1;
    return 0;
}

int barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&(barrier->mtx));
    while (barrier->times_used != 1)
        pthread_cond_wait(&(barrier->cv), &(barrier->mtx));
    ++(barrier->count);
    // all threads are done
    if (barrier->count == barrier->n_threads) {
        barrier->times_used = 0;
        --(barrier->count);
        pthread_cond_broadcast(&(barrier->cv));
    }
    // there are still threads running
    else {
        while (barrier->count != barrier->n_threads && barrier->times_used == 1)
            pthread_cond_wait(&(barrier->cv), &(barrier->mtx));
        --(barrier->count);
        if (barrier->count == 0)
            barrier->times_used = 1;
        pthread_cond_broadcast(&(barrier->cv));
    }
    pthread_mutex_unlock(&(barrier->mtx));
    return 0;
}
