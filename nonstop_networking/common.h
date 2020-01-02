/**
* Networking Lab
* CS 241 - Fall 2018
*/

#pragma once
#include <stddef.h>
#include <sys/types.h>

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

/**
 * Represent size_t with a 8 bytes string(little-endian)
 */
char *size_to_string(size_t size);

/**
 * Convert size_string(little-endian) to size_t
 */
size_t string_to_size(char *size_str);

/**
 * Shift contents in buffer to the left with given offset"
 */
void shift_forward(char *buffer, size_t content_size, size_t offset);