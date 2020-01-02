/**
 * Networking Lab
 * CS 241 - Fall 2018
 */

#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *size_to_string(size_t size) {
    char *size_str = malloc(9);
    for (size_t i = 0; i < 8; ++i) {
        size_str[i] = size & 0xFF;
        size = size >> 8;
    }
    size_str[8] = '\0';
    return size_str;
}

size_t string_to_size(char *size_str) {
    size_t size = 0;
    for (size_t i = 0; i < 8; ++i) {
        size += ((unsigned char)size_str[i]) << (i * 8);
    }
    return size;
}

void shift_forward(char *buffer, size_t content_size, size_t offset) {
    if (offset == content_size) {
        *buffer = '\0';
    } else if (offset == 0) {
        return;
    } else {
        char temp[content_size - offset];
        memcpy(temp, buffer + offset, content_size - offset);
        memcpy(buffer, temp, content_size - offset);
    }
}
