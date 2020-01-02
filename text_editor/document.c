/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "document.h"
#include "vector.h"

struct document {
    vector *vector;
};

document *document_create() {
    document *this = (document *)malloc(sizeof(document));
    assert(this);
    this->vector = vector_create(string_copy_constructor, string_destructor,
                                 string_default_constructor);
    return this;
}

void document_write_to_file(document *this, const char *path_to_file) {
    assert(this);
    assert(path_to_file);
    // TODO: your code here!
    int fd = open(path_to_file, O_RDWR | O_CREAT | O_TRUNC, 0666);
    assert(fd != -1);
    size_t size = vector_size(this->vector);
    for (size_t i = 0; i < size; ++i) {
        dprintf(fd, "%s\n", (char*) vector_get(this->vector, i));
    }
    close(fd);
}

document *document_create_from_file(const char *path_to_file) {
    assert(path_to_file);
    // TODO: your code here!
    document *doc = document_create();
    FILE *file = fopen(path_to_file, "r");
    if (file == NULL) {
        return doc;
    }
    char *line = NULL;
    size_t capacity = 0;
    ssize_t byte_read = 0;
    while (1) {
        byte_read = getline(&line, &capacity, file);
        if (byte_read == -1)
            break;
        // remove '\n' if there is any
        if (byte_read > 0 && line[byte_read - 1] == '\n')
            line[byte_read - 1] = '\0';
        vector_push_back(doc->vector, (void*) line);
    }
    free(line);
    fclose(file);
    return doc;
}

void document_destroy(document *this) {
    assert(this);
    vector_destroy(this->vector);
    free(this);
}

size_t document_size(document *this) {
    assert(this);
    return vector_size(this->vector);
}

void document_set_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    size_t index = line_number - 1;
    vector_set(this->vector, index, (void *)str);
}

const char *document_get_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    return (const char *)vector_get(this->vector, index);
}

void document_insert_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    // TODO: your code here!
    assert(line_number > 0);
    size_t num_line = vector_size(this->vector);
    if (line_number <= num_line) {
        vector_insert(this->vector, line_number - 1, (void*) str);
    } else {
        for (size_t i = 1; i < line_number - num_line; ++i) {
            vector_push_back(this->vector, "");
        }
        char *str_cpy = malloc(strlen(str) + 1);
        strcpy(str_cpy, str);
        vector_push_back(this->vector, str_cpy);
        free(str_cpy);
    }
}

void document_delete_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    vector_erase(this->vector, index);
}
