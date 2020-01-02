/**
* Know Your Tools Lab
* CS 241 - Fall 2018
*/

#pragma once

#include <sys/types.h>

typedef struct {
    /*
    * The length of the lines array
    */
    size_t num_lines;

    /*
    * lines[i] represents the ith line,
    * i is zero indexed
    */
    char **lines;
} file;

/*
* Reads and wraps a file to 80 characters into memory
*/
file *read_wrap(const char *filename);

/*
* Cleans up all resources with a file
*/
void destroy_file(file *ptr);

/*
* Adds a line to a file. Capacity is the number of strings
* that lines has space for, meaning
* input->lines = malloc(sizeof(char*)*capacity)
* Returns the new capacity of the input->lines array
* If the array size needs to be altered.
*/
size_t add_line(file *input, size_t capacity, char *line);

/*
* Creates an empty new file
*/
file *new_file(size_t capacity);