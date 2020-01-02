/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once

#include "vector.h"

/**
 * Rule Data Structure
 */
typedef struct {
    char *target;     // Name of target
    vector *commands; // vector of pointers to char arrays
    int state;        // State of the rule. Defaults to 0
    void *data;       // Anything user wishes to add (memory not managed)
} rule_t;

/**
 * Initializes Rule data structure, which initializes the `commands` vector
 * to an empty string vector which manages deep copies of strings,
 * initializes `state` to 0, `data` to NULL, and `target` to NULL.
 */
void rule_init(rule_t *rule);

/**
 * Destroys rule data structure, freeing `target` string, destroying
 * `commands` vector, and freeing struct memory.
 */
void rule_destroy(rule_t *rule);

/**
 * Soft copies a rule into another rule. Doesn't deep copy vector, string,
 * or data.
 */
void rule_soft_copy(rule_t *rule, rule_t *cpy_rule);
