/**
* Teaching Threads Lab
* CS 241 - Fall 2018
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

typedef struct reduce_t {
    int *list;
    size_t list_len;
    reducer reduce_func;
    int base_case;
} reduce_t;

size_t *num_elem_in_each_thread(size_t, size_t);
void *start_rountine(void *); 

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case, size_t num_threads) {
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    int *retvals = malloc(num_threads * sizeof(int));
    reduce_t *data = malloc(num_threads * sizeof(reduce_t));
    size_t *sizes = num_elem_in_each_thread(list_len, num_threads);
    size_t offset = 0;
    size_t num_running_threads = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        if (sizes[i] > 0) {
            data[i] = (reduce_t){list + offset, sizes[i], reduce_func, base_case};
            pthread_create(threads + i, NULL, start_rountine, data + i);
            offset += sizes[i];
            ++num_running_threads;
        }
    }
    for (size_t i = 0; i < num_running_threads; ++i) {
        void *retval = NULL;
        pthread_join(threads[i], &retval);
        retvals[i] = *((int *)retval);
        free(retval);
    }
    int result = reduce(retvals, num_running_threads, reduce_func, base_case);
    free(threads);
    free(retvals);
    free(data);
    free(sizes);
    return result;
}


size_t *num_elem_in_each_thread(size_t list_len, size_t num_threads) {
    size_t *result = malloc(sizeof(size_t*) * num_threads);
    size_t base = list_len / num_threads;
    size_t remain = list_len % num_threads;
    size_t count = 0;
    while(count < num_threads) {
        result[count] = base;
        if (count < remain)
            result[count]++;
        ++count;
    }
    return result;
}


void *start_rountine(void *data) {
    reduce_t *temp = (reduce_t*)data;
    int *result = malloc(sizeof(int));
    *result = reduce(temp->list, temp->list_len, temp->reduce_func, temp->base_case);
    return (void*) result;
}