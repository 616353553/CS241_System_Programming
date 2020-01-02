/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include <stdio.h>
#include <crypt.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct craker_t {
    pthread_t tid;
    size_t index;
    int hash_count;
} cracker_t;


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier_0, barrier_1;
static char username[9], hash[14], password[9];
static int has_task;
static char* recoverd_pw;
static size_t num_threads;

void *crack(void *);

int start(size_t thread_count) {
    num_threads = thread_count;
    pthread_barrier_init(&barrier_0, NULL, thread_count + 1);
	pthread_barrier_init(&barrier_1, NULL, thread_count + 1);
    // initialize crackers
    cracker_t *crackers = malloc(thread_count * sizeof(cracker_t));
    for (size_t i = 0; i < thread_count; ++i) {
        crackers[i] = (cracker_t){0, i + 1, 0};
        pthread_create(&(crackers[i].tid), NULL, crack, crackers + i);  
    }
    char *line = NULL;
    size_t size = 0;
    // read inputs
    while (getline(&line, &size, stdin) != -1) {
        has_task = 1;
        recoverd_pw = NULL;
        sscanf(line, "%s %s %s\n", username, hash, password);
        pthread_mutex_lock(&lock);
        v2_print_start_user(username);
        pthread_mutex_unlock(&lock);
        // start cracking now!!
        pthread_barrier_wait(&barrier_0);
        double start_time = getTime();
        double cpu_start_time = getCPUTime();
        // get actual password..
        pthread_barrier_wait(&barrier_1);
        int total_hash_count = 0;
        for (size_t i = 0; i < thread_count; ++i)
            total_hash_count += crackers[i].hash_count;
        int is_not_found = (recoverd_pw == NULL) ? 1 : 0;
        pthread_mutex_lock(&lock);
        v2_print_summary(username, recoverd_pw, total_hash_count, getTime() - start_time,
                        getCPUTime() - cpu_start_time, is_not_found);
        pthread_mutex_unlock(&lock);
        free(recoverd_pw);
    }
    // no more password to crack...
    has_task = 0;
    pthread_barrier_wait(&barrier_0);
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_join(crackers[i].tid, NULL);
    }
    pthread_barrier_destroy(&barrier_0);
    pthread_barrier_destroy(&barrier_1);
    free(crackers);
    free(line);
    return 0;
}

void *crack(void *data) {
    cracker_t *cracker = data;
    while (1) {
        pthread_barrier_wait(&barrier_0);
        if (has_task == 0) {
            break;
        }
        int prefix_len = getPrefixLength(password);
        char *start_pw = strdup(password);
        long start_index, count = 0;
        getSubrange(strlen(password) - prefix_len, num_threads, cracker->index, &start_index, &count);
        setStringPosition(start_pw + prefix_len, start_index);
        pthread_mutex_lock(&lock);
        v2_print_thread_start(cracker->index, username, start_index, start_pw);
        pthread_mutex_unlock(&lock);
        struct crypt_data cdata;
        cdata.initialized = 0;
        int status = 2;
        for (long i = 0; i < count; ++i) {
            if (recoverd_pw != NULL) {
                status = 1;
                break;
            }
            cracker->hash_count++;
            if (strcmp(crypt_r(start_pw, "xx", &cdata), hash) == 0) {
                status = 0;
                recoverd_pw = start_pw;
                break;
            }
            incrementString(start_pw);
        }
        if (status == 1 || status == 2) {
            free(start_pw);
        }
        pthread_mutex_lock(&lock);
        v2_print_thread_result(cracker->index, cracker->hash_count, status);
        pthread_mutex_unlock(&lock);
        pthread_barrier_wait(&barrier_1);
    }
    return NULL;
}
