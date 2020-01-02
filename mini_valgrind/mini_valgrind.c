/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


meta_data *previous_entry(meta_data *);
int is_invalid_addr(void *); 

meta_data *head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename, void *instruction) {
    if (request_size == 0)
        return NULL;
    // allocate momory for meta_data and user memory
    meta_data *entry = malloc(sizeof(meta_data) + request_size);
    if (entry == NULL)
        return NULL;
    // set entry values
    entry->request_size = request_size;
    entry->filename = filename;
    entry->instruction = instruction;
    entry->next = NULL;
    // append entry to the linked list
    if (head == NULL)
        head = entry;
    else {
        meta_data *curr = head;
        while(curr->next != NULL)
            curr = curr->next;
        curr->next = entry;
    }
    // increment total_memory_requested
    total_memory_requested += request_size;
    return (void*)(entry + 1);
}

void *mini_calloc(size_t num_elements, size_t element_size, const char *filename, void *instruction) {
    size_t request_size = num_elements * element_size;
    void *ptr = mini_malloc(request_size, filename, instruction);
    for (size_t i = 0; i < request_size; ++i)
        *((char*)ptr + i) = 0;
    return ptr;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename, void *instruction) {
    if (request_size == 0) {
        mini_free(payload);
        return NULL;
    }
    if (is_invalid_addr(payload)) {
        invalid_addresses++;
        return NULL;
    }
    if (payload == NULL)
        return mini_malloc(request_size, filename, instruction);
    meta_data *entry = (meta_data *)payload - 1;
    meta_data *prev_entry = previous_entry(entry);
    meta_data *new_entry = realloc(entry, request_size + sizeof(meta_data));
    if (new_entry == NULL)
        return NULL;
    if (new_entry != entry) {
        if (prev_entry != NULL)
            prev_entry->next = new_entry;
        else
            head = new_entry;
    }
    if (request_size > new_entry->request_size)
        total_memory_requested += request_size - new_entry->request_size;
    else
        total_memory_freed += new_entry->request_size - request_size;
    new_entry->request_size = request_size;
    return (void *)(new_entry + 1);
}

void mini_free(void *payload) {
    if (payload == NULL)
        return;
    if (is_invalid_addr(payload)) {
        invalid_addresses++;
        return;
    }
    meta_data *entry = ((meta_data *)payload) - 1;
    meta_data *prev_entry = previous_entry(entry);
    if (prev_entry != NULL)
        prev_entry->next = entry->next;
    else
        head = entry->next;
    total_memory_freed += entry->request_size;
    free(entry);
}

meta_data *previous_entry(meta_data *entry) {
    if (head == NULL || entry == head)
        return NULL;
    meta_data *curr = head;
    while(curr->next != entry)
        curr = curr->next;
    return curr;
}

int is_invalid_addr(void *ptr) {
    if (ptr == NULL)
        return 0;
    if (head == NULL)
        return 1;
    meta_data *curr = head;
    while(curr) {
        if ((void*)(curr + 1) == ptr)
            return 0;
        curr = curr->next;
    }
    return 1;
}
