/**
* Know Your Tools Lab
* CS 241 - Fall 2018
*/

#include "read_wrap.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t add_line(file *input, size_t capacity, char *line) {
    if (input->num_lines == capacity) {
        capacity *= 2;
        input->lines = realloc(input->lines, sizeof(char *) * capacity);
    }
    input->lines[input->num_lines++] = strdup(line);
    return capacity;
}

file *new_file(size_t capacity) {
    file *output = malloc(sizeof(*output));
    *output = (file){0, malloc(sizeof(char *) * capacity)};
    return output;
}

void destroy_file(file *ptr) {
    for (size_t i = 0; i < ptr->num_lines; ++i) {
        free(ptr->lines[i]);
    }
    free(ptr->lines);
    free(ptr);
}

file *read_wrap(const char *filename) {
    FILE *file_obj = fopen(filename, "r");
    if (file_obj == NULL) {
        return NULL;
    }
    size_t capacity = 10;
    file *output = new_file(capacity);
    char *file_buf = NULL;
    size_t n;
    ssize_t read = 0;
    size_t lines = 0;
    while ((read = getline(&file_buf, &n, file_obj)) != -1) {
        char *newline = strchr(file_buf, '\n');
        if (newline) {
            *newline = '\0';
            read--;
        }
        char *wrapped_string = file_buf;
        ssize_t limit = 80;
        while (read > limit) {
            char saved = wrapped_string[limit];
            wrapped_string[limit] = '\0';
            // Add a breakpoint here! Make sure that the
            // wrapped_string is actually 80 characters
            capacity = add_line(output, capacity, wrapped_string);
            lines++;
            wrapped_string[limit] = saved;
            wrapped_string += limit;
            read -= limit;
        }
        if (read > 0) {
            capacity = add_line(output, capacity, wrapped_string);
            lines++;
        }
    }
    output->lines = realloc(output->lines, sizeof(char *) * lines);
    free(file_buf);
    fclose(file_obj);
    return output;
}