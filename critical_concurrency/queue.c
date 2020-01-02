/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    queue *this = malloc(sizeof(queue));
    if (this == NULL)
        return NULL;
    this->head = NULL;
    this->tail = NULL;
    this->size = 0;
    this->max_size = max_size;
    pthread_cond_init(&(this->cv), NULL);
    pthread_mutex_init(&(this->m), NULL);
    return this;
}

void queue_destroy(queue *this) {
    if (this == NULL)
        return;
    queue_node *curr = this->head;
    while (curr != NULL) {
        queue_node *temp = curr;
        curr = curr->next;
        free(temp);
    }
    pthread_cond_destroy(&(this->cv));
    pthread_mutex_destroy(&(this->m));
    free(this);
}

void queue_push(queue *this, void *data) {
    pthread_mutex_lock(&(this->m));
    if (this->max_size > 0) {
        while (this->size == this->max_size)
            pthread_cond_wait(&(this->cv), &(this->m));
    }
    queue_node *node = malloc(sizeof(queue_node));
    node->data = data;
    node->next = NULL;

    if (this->size == 0) {
        this->head = node;
        this->tail = node;
    } else {
        this->tail->next = node;
        this->tail = node;
    }
    ++(this->size);
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
}

void *queue_pull(queue *this) {
    pthread_mutex_lock(&(this->m));
    while (this->head == NULL)
        pthread_cond_wait(&(this->cv), &(this->m));
    queue_node *node = this->head;
    if (this->head == this->tail) {
        this->head = NULL;
        this->tail = NULL;
    } else {
        this->head = node->next;
    }
    --(this->size);
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
    void *data = node->data;
    free(node);
    return data;
}
