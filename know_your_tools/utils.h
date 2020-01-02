/**
* Know Your Tools Lab
* CS 241 - Fall 2018
*/

#pragma once

#include "read_wrap.h"
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
* Returns the minimum of the two values
*/
#define min(a, b) a < b ? a : b

/*
* Returns the size of the file in bytes
* -1 if an error occurred, errno is set
*/
ssize_t file_size(int fd);

/*
* Moves a valid file descriptor back to the
* beginning of the file
*/
void reset_file(int fd);

/*
* Writes the first five lines and a newline
* to the file descriptor 'fd'
*/
void print_head(file *input, int fd);

/*
* Writes the last five lines and a newline
* to the file descriptor 'fd'
*/
void print_tail(file *input, int fd);

/*
* Writes the entire file to 'fd'
*/
void print_entire_file(file *input, int fd);

/*
* Address printing functions
*/
void print_main(void *addr);

void print_strdup_addr(void *addr);

void print_string_literal_addr(void *addr);

void print_heap_addr(void *addr);

void print_argc_addr(void *addr);

void print_static_variable_addr(void *addr);

extern const size_t NUM_LINES_HEAD;
extern const size_t NUM_LINES_TAIL;