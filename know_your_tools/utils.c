/**
* Know Your Tools Lab
* CS 241 - Fall 2018
*/

#include "utils.h"
#include <stdio.h>
#include <unistd.h>

ssize_t file_size(int fd) {
    /*
     * Hmm.. I wonder if this system calls is working
     * try `man fstat` and read about it
     */
    struct stat buf;
    if (fstat(fd, &buf) == -1) {
        perror("fstat");
        return -1;
    }
    return (ssize_t)buf.st_size;
}

void reset_file(int fd) {
    lseek(fd, 0, SEEK_SET);
}

void print_head(file *input, int fd) {
    for (size_t i = 0; i < NUM_LINES_HEAD; ++i) {
        dprintf(fd, "%s\n", input->lines[i]);
    }
    dprintf(fd, "\n");
}

void print_tail(file *input, int fd) {
    for (size_t i = input->num_lines - NUM_LINES_TAIL; i < input->num_lines;
         ++i) {
        dprintf(fd, "%s\n", input->lines[i]);
    }
    dprintf(fd, "\n");
}

void print_entire_file(file *input, int fd) {
    for (size_t i = 0; i < input->num_lines; ++i) {
        dprintf(fd, "%s\n", input->lines[i]);
    }
}

static void print_memory_location(const char *what, void *where) {
    printf("%p - %s\n", where, what);
}

void print_main(void *addr) {
    print_memory_location("main", addr);
}

void print_strdup_addr(void *addr) {
    print_memory_location("strdup", addr);
}

void print_string_literal_addr(void *addr) {
    print_memory_location("string_literal", addr);
}

void print_heap_addr(void *addr) {
    print_memory_location("heap", addr);
}

void print_argc_addr(void *addr) {
    print_memory_location("argc", addr);
}