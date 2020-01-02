/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <string.h>
#ifdef DEBUG
#include <signal.h>
#include <stdio.h>
#endif

#include "callbacks.h"
#include "rule.h"
#include "vector.h"

void rule_init(rule_t *rule) {
    assert(rule != NULL);
    rule->target = NULL;
    rule->commands = vector_create(string_copy_constructor, string_destructor,
                                   string_default_constructor);
    rule->state = 0;
    rule->data = NULL;
}

void rule_destroy(rule_t *rule) {
    if (!rule)
        return;
    string_destructor(rule->target); // Can free null :)
    vector_destroy(rule->commands);
    free(rule);
}

void rule_soft_copy(rule_t *dest, rule_t *src) {
    assert(src != NULL);
    memcpy(dest, src, sizeof(*src));
}
