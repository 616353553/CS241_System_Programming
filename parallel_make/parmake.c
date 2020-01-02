/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/


#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include <stdbool.h>
#include "vector.h"
#include "dictionary.h"
#include "queue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

int has_cycle(void*);
int detect_cycle(dictionary*, void*);
void get_ordered_rules(vector*);
void get_rules(vector*, vector*, dictionary*);
void update_dictionary(dictionary*, void*, int);
int should_run(void*);
void *run(void*);

graph *g = NULL;
pthread_cond_t graph_cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t graph_lock = PTHREAD_MUTEX_INITIALIZER;

vector *rules = NULL;
// size_t num_rules = 0;
pthread_mutex_t rule_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rule_cv = PTHREAD_COND_INITIALIZER;


int parmake(char *makefile, size_t num_threads, char **targets) {
    g = parser_parse_makefile(makefile, targets);
    if (num_threads < 1) {
        return 0;
    }
    // vector of targets
    vector *target_vector= graph_neighbors(g, "");
    size_t num_targets = vector_size(target_vector);
    // look for cycles
    bool cycle_found = false;
    for (size_t i = 0; i < num_targets; ++i) {
        void *curr = vector_get(target_vector, i);
        if (has_cycle(curr) == 1) {
            print_cycle_failure((char*) curr);
            cycle_found = true;
        }
    }
    // load rules into queue
    if (!cycle_found) {
        rules = shallow_vector_create();
        get_ordered_rules(target_vector);
        // for (size_t i = 0; i < num_rules; ++i) {
        //     printf("rule %zu: %s\n", i, (char*)queue_pull(rules));
        //     fflush(stdout);
        // }
        // create threads
        pthread_t threads[num_threads]; 
        for (size_t i = 0; i < num_threads; ++i) {
            // failed to create new process
            if (pthread_create(&threads[i], NULL, run, NULL) != 0)
                exit(1);
        }
        for (size_t i = 0; i < num_threads; ++i) {
            // failed to join process
            if (pthread_join(threads[i], NULL) != 0)
                exit(1);
        }
        vector_destroy(rules);
    }
    graph_destroy(g);
    vector_destroy(target_vector);
    return 0;
}


int has_cycle(void *target) {
    if (g == NULL)
        return -1;
    if (!graph_contains_vertex(g, target))
        return -1;
    dictionary *history = string_to_int_dictionary_create();
    vector *keys = graph_vertices(g);
    size_t num_keys = vector_size(keys);
    for (size_t i = 0; i < num_keys; ++i) {
        int value = 0;
        dictionary_set(history, vector_get(keys, i), &value);
    }
    int result = detect_cycle(history, target);
    dictionary_destroy(history);
    vector_destroy(keys);
    return result;
}


int detect_cycle(dictionary *history, void *target) {
    int *state = dictionary_get(history, target);
    // target is in progress, which means cycle is found
    if (*state == 1)
        return 1;
    // target is done   
    if (*state == 2)
        return 0;
    // set state to "in progress"
    update_dictionary(history, target, 1);
    // check all of its descendants
    vector *neighbors = graph_neighbors(g, target);
    size_t num_neighbors = vector_size(neighbors);
    for (size_t i = 0; i < num_neighbors; ++i) {
        if (detect_cycle(history, vector_get(neighbors, i)) == 1) {
            vector_destroy(neighbors);
            return 1;
        }
    }
    // set state to "finished"
    update_dictionary(history, target, 2);
    vector_destroy(neighbors);
    return 0;
}


void get_ordered_rules(vector *targets) {
    dictionary *counter = string_to_int_dictionary_create();
    vector *keys = graph_vertices(g);
    size_t num_keys = vector_size(keys);
    for (size_t i = 0; i < num_keys; ++i) {
        int value = 0;
        dictionary_set(counter, vector_get(keys, i), &value);
    }
    get_rules(rules, targets, counter);
    vector_destroy(keys);
    dictionary_destroy(counter);
}


void get_rules(vector *result, vector *targets, dictionary *counter) {
    size_t num_targets = vector_size(targets);
    for (size_t i = 0; i < num_targets; ++i) {
        void *target = vector_get(targets, i);
        vector *sub_targets = graph_neighbors(g, target);
        get_rules(result, sub_targets, counter);
        if (*((int*)dictionary_get(counter, target)) == 0) {
            update_dictionary(counter, target, 1);
            vector_push_back(result, target);
        }
        vector_destroy(sub_targets);
    }
}


void update_dictionary(dictionary *dict, void *key, int new_value) {
    int *value = dictionary_get(dict, key);
    *value = new_value;
}


// -1: error found, should not run, and need to be marked as fail
//  0: dependencies not finished yet, check later
//  1: all dependecies are satisfied, should run immediately
//  2: should not run, and need to be marked as satisfied
//  3: rule is already done, don't do anything
int should_run(void *target) {
    rule_t *rule = (rule_t *)graph_get_vertex_value(g, target);
    if (rule->state != 0) {
        return 3;
    }
    vector *sub_targets = graph_neighbors(g, target);
    size_t num_sub_targets = vector_size(sub_targets);
    if (num_sub_targets > 0) {
        // if target is file
        if (access(target, F_OK) != -1) {
            for (size_t i = 0; i < num_sub_targets; ++i) {
                char *sub_target = vector_get(sub_targets, i);
                // if sub-target is also a file, compare time
                if (access(sub_target, F_OK) != -1) {
                    struct stat stat_0, stat_1;
                    // failed to read file's stat
	                if (stat((char *)target, &stat_0) == -1 || stat(sub_target, &stat_1) == -1) {
                        vector_destroy(sub_targets);
                        return -1;
                    }   
                    // if sub-target is newer than target
	                if (difftime(stat_0.st_mtime, stat_1.st_mtime) < 0) {
                        vector_destroy(sub_targets);
                        return 1;
                    }
                } else {
                    vector_destroy(sub_targets);
                    return 1;
                }
            }
            vector_destroy(sub_targets);
            return 2;
        } else {
            // check if all sub-rules succeed
            pthread_mutex_lock(&graph_lock);
            for (size_t i = 0; i < num_sub_targets; ++i) {
                rule_t *sub_rule = graph_get_vertex_value(g, vector_get(sub_targets, i));
                int state = sub_rule->state;
                if (state != 1) {
                    pthread_mutex_unlock(&graph_lock);
                    vector_destroy(sub_targets);
                    return state;
                }
            }
            pthread_mutex_unlock(&graph_lock);
            vector_destroy(sub_targets);
            return 1;
        }
    } else {
        vector_destroy(sub_targets);
        return access(target, F_OK) != -1 ? 2 : 1;
    }
}



void *run(void *data) {
    (void) data;
    while (true) {
        pthread_mutex_lock(&rule_lock);
        size_t num_rules = vector_size(rules);
        if (num_rules > 0) {
            for (size_t i = 0; i < num_rules; ++i) {
                void *target = vector_get(rules, i);
                int status = should_run(target);
                rule_t *rule = graph_get_vertex_value(g, target);
                if (status == 1) {
                    vector_erase(rules, i);
                    pthread_mutex_unlock(&rule_lock);
                    vector *commands = rule->commands;
                    size_t num_commands = vector_size(commands);
                    int new_state = 1;
                    for (size_t i = 0; i < num_commands; ++i) {
                        if (system((char *)vector_get(commands, i)) != 0) {
                            new_state = -1;
                            break;
	                    }
                    }
                    pthread_mutex_lock(&graph_lock);
                    rule->state = new_state;
                    pthread_cond_broadcast(&rule_cv);
                    pthread_mutex_unlock(&graph_lock);
                    break;
                } else if (status == -1 || status == 2) {
                    vector_erase(rules, i);
                    pthread_mutex_unlock(&rule_lock);
                    pthread_mutex_lock(&graph_lock);
                    rule->state = status == -1 ? -1 : 1;
                    pthread_cond_broadcast(&rule_cv);
                    pthread_mutex_unlock(&graph_lock);
                    break;
                } else if (status == 3) {
                    vector_erase(rules, i);
                    pthread_mutex_unlock(&rule_lock);
                    break;
                } else if (i == num_rules - 1) {
                    pthread_cond_wait(&rule_cv, &rule_lock);
                    pthread_mutex_unlock(&rule_lock);
                    break;
                }
            }
        } else {
            pthread_mutex_unlock(&rule_lock);
            return NULL;
        }
        
        // int status = should_run(target);
        // while (status == 0) {
        //     pthread_mutex_lock(&graph_lock);
        //     pthread_cond_wait(&graph_cv, &graph_lock);
        //     pthread_mutex_unlock(&graph_lock);
        //     status = should_run(target);
        // }
        // if (status == 1) {
        //     vector *commands = rule->commands;
        //     size_t num_commands = vector_size(commands);
        //     for (size_t i = 0; i < num_commands; ++i) {
        //         if (system((char *)vector_get(commands, i)) != 0) {
        //             pthread_mutex_lock(&graph_lock);
        //             rule->state = -1;
        //             pthread_cond_broadcast(&graph_cv);
        //             pthread_mutex_unlock(&graph_lock);
        //             break;
	    //         }
        //     }
        // } else if (status == -1 || status == 2) {
        //     pthread_mutex_lock(&graph_lock);
        //     rule->state = status == -1 ? -1 : 1;
        //     pthread_cond_broadcast(&graph_cv);
        //     pthread_mutex_unlock(&graph_lock);
        // }




        // rule_t *rule = graph_get_vertex_value(g, target);
        // while (true) {
        //     int status = should_run(target);
        //     if (status == 0) {
        //         pthread_mutex_lock(&graph_lock);
        //         pthread_cond_wait(&graph_cv, &graph_lock);
        //         pthread_mutex_unlock(&graph_lock);
        //     } else if (status == 1) {
        //         pthread_mutex_unlock(&graph_lock);
        //         vector *commands = rule->commands;
        //         size_t num_commands = vector_size(commands);
        //         int new_state = 1;
        //         for (size_t i = 0; i < num_commands; ++i) {
        //             if (system((char *)vector_get(commands, i)) != 0) {
        //                 new_state = -1;
        //                 break;
	    //             }
        //         }
        //         pthread_mutex_lock(&graph_lock);
        //         rule->state = new_state;
        //         pthread_cond_broadcast(&graph_cv);
        //         pthread_mutex_unlock(&graph_lock);
        //         break;
        //     } else if (status == 3) {
        //         pthread_mutex_unlock(&graph_lock);
        //         break;
        //     } else {
        //         rule->state = status == -1 ? -1 : 1;
        //         pthread_cond_broadcast(&graph_cv);
        //         pthread_mutex_unlock(&graph_lock);
        //         break;
        //     }
        // }
    }
}


