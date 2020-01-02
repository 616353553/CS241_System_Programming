/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    char *str;
};

sstring *cstr_to_sstring(char *input) {
    sstring *sstr = malloc(sizeof(sstring));
    char *temp = malloc(strlen(input) + 1);
    strcpy(temp, input);
    sstr->str = temp;
    return sstr;
}

char *sstring_to_cstr(sstring *input) {
    char *temp = malloc(strlen(input->str) + 1);
    strcpy(temp, input->str);
    return temp;
}

int sstring_append(sstring *this, sstring *addition) {
    this->str = realloc(this->str, strlen(this->str) + strlen(addition->str) + 1);
    strcpy(this->str + strlen(this->str), addition->str);
    return strlen(this->str);
}

vector *sstring_split(sstring *this, char delimiter) {
    vector *v = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    char *start = this->str;
    while(start < this->str + strlen(this->str)) {
        char *end = strchr(start, delimiter);
        if (!end) {
            vector_push_back(v, start);
            break;
        }
        char temp = *end;
        *end = '\0';
        vector_push_back(v, start);
        *end = temp;
        start = end + 1;
    }
    return v;
}

int sstring_substitute(sstring *this, size_t offset, char *target, char *substitution) {
    if (offset > strlen(this->str))
        return -1;
    char *pos = strstr(this->str + offset, target);
    if (pos != NULL) {
        char *temp = malloc(strlen(this->str) + strlen(substitution) - strlen(target) + 1);
        strncpy(temp, this->str, pos - this->str);
        strcpy(temp + (pos - this->str), substitution);
        strcpy(temp + (pos - this->str) + strlen(substitution), pos + strlen(target));
        free(this->str);
        this->str = temp;
        return 0;
    } else {
        return -1;
    }
}

char *sstring_slice(sstring *this, int start, int end) {
    char * slice = malloc(end - start + 1);
    strncpy(slice, this->str + start, end - start);
    slice[end - start] = '\0';
    return slice;
}

void sstring_destroy(sstring *this) {
    free(this->str);
    free(this);
}
