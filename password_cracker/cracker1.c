/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <crypt.h>

typedef struct craker_t {
    pthread_t tid;
    size_t index;
} cracker_t;


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int recoverd = 0;
static queue *tasks = NULL;
static cracker_t *crackers = NULL;
static int total = 0;

void *crack(void*);

int start(size_t thread_count) {
    // create tasks
    tasks = queue_create(0);
    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, stdin) != -1) {
        queue_push(tasks, strdup(line));
        ++total;
    }
    queue_push(tasks, NULL);
    // create crackers
    crackers = malloc(thread_count * sizeof(cracker_t));
    for (size_t i = 0; i < thread_count; ++i) {
        crackers[i].index = i + 1;
        pthread_create(&(crackers[i].tid), NULL, crack, crackers + i);
    }
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_join(crackers[i].tid, NULL);
    }
    v1_print_summary(recoverd, total - recoverd);
    free(crackers);
    free(line);
    queue_destroy(tasks);
    return 0;
}


void *crack(void *data) {
    cracker_t *cracker = data;
    char *task = NULL;
    char username[9], hash[14], password[9];
    while ((task = queue_pull(tasks)) != NULL) {
        sscanf(task, "%s %s %s", username, hash, password);
        // print start info
        v1_print_thread_start(cracker->index, username);
        double start_time = getThreadCPUTime();
        int prefix_len = getPrefixLength(password);
        int hash_count = 0;
        int is_not_found = 1;
        if (prefix_len == (int)strlen(password)) {
            is_not_found = 0;
        } else {
            int suffix_len = strlen(password) - prefix_len;
            struct crypt_data cdata;
            cdata.initialized = 0;
            setStringPosition(password + prefix_len, 0);
            int range = 1;
            for (int i = 0; i < suffix_len; ++i)
                range *= 26;
            for (int i = 0; i < range; ++i) {
                ++hash_count;
                if (strcmp(crypt_r(password, "xx", &cdata), hash) == 0) {
                    // update recovered password counter
                    pthread_mutex_lock(&lock);
                    ++recoverd;
                    pthread_mutex_unlock(&lock);
                    is_not_found = 0;
                    break;
                }
                incrementString(password);
            }
        }
        // print result
        v1_print_thread_result(cracker->index, username, password, 
                        hash_count, getThreadCPUTime() - start_time, is_not_found);
        free(task);
        task = NULL;
    }
    queue_push(tasks, NULL);
    free(task);
    return NULL;
}






