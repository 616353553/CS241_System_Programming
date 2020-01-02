/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include "camelCaser.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static char **split_str(const char *str) {
    char **parts = NULL;
    int num_parts = 0;
    size_t i = 0;
    while (i < strlen(str)) {
        size_t j = i;
        for ( ; j < strlen(str); ++j) {
            if (ispunct(str[j])) {
                char *part = malloc(j - i + 1);
                strncpy(part, str + i, j - i);
                part[j++ - i] = '\0';
                parts = realloc(parts, ++num_parts * sizeof(char*));
                parts[num_parts - 1] = part;
                break;
            }
        }
        i = j;
    }
    parts = realloc(parts, (num_parts + 1) * sizeof(char*));
    parts[num_parts] = NULL;
    return parts;
}


static void camel_case_word(char *word, bool is_first_word) {
    bool first_letter_found = false;
    for (size_t i = 0; i < strlen(word); ++i) {
        if (isalpha(word[i])) {
            if (!is_first_word && !first_letter_found) {
                if (word[i] >= 'a' && word[i] <= 'z') {
                    word[i] -= 32;
                }
                first_letter_found = true;
            } else {
                if(word[i] >= 'A' && word[i] <= 'Z') {
                    word[i] += 32;
                }
            }
        }
    }
}


static void camel_case_str(char *str) {
    size_t i = 0;
    while(i < strlen(str)) {
        bool start_word = false;
        size_t j = i;
        for ( ; j < strlen(str); ++j) {
            if ((isspace(str[j]) && start_word)) {
                char temp = str[j];
                str[j]='\0';
                camel_case_word(str + i, i == 0);
                str[j++] = temp;
                break;
            }
            if (j == strlen(str) - 1) {
                camel_case_word(str + i, i == 0);
                ++j;
                break;
            }
            if (!isspace(str[j]) && !start_word) {
                start_word = true;
            }
        }
        i = j;
    }
}

static void remove_spaces(char **str) {
    char *new_str = malloc(strlen(*str) + 1);
    size_t count = 0;
    for(size_t i = 0; i < strlen(*str); ++i) {
        if (!isspace((*str)[i])) {
            new_str[count++] = (*str)[i];
        }
    }
    new_str[count] = '\0';
    free(*str);
    *str = new_str;
}

char **camel_caser(const char *input_str) {
    if (input_str == NULL)
        return NULL;
    char **parts = split_str(input_str);
    char **curr = parts;
    while(*curr) {
        camel_case_str(*curr);
        remove_spaces(curr);
        curr++;
    }
    return parts;
}

void destroy(char **result) {
    if (!result) {
        return;
    }
    char **curr = result;
    while(*curr) {
        free(*curr);
        curr++;
    }
    free(result);
}