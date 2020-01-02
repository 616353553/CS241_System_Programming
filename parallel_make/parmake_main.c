/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include "parmake.h"

#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void parse_args(int argc, char **argv, char **makefile_ref,
                       size_t *num_threads_ref, char ***targets_ref) {
    int c;
    char *invalid_ptr;
    long value;
    // Parse the flags and arguments using getopt
    while ((c = getopt(argc, argv, ":f:j:")) != -1) {
        switch (c) {
        case 'f':
            *makefile_ref = optarg;
            break;
        case 'j':
            value = strtol(optarg, &invalid_ptr, 10);
            if (*invalid_ptr || value < 1) {
                fprintf(stderr, "parmake: the '-j' option requires a positive "
                                "integer argument\n");
                exit(2);
            }
            *num_threads_ref = value;
            break;
        }
    }

    *targets_ref = argv + optind;

    // Default to "makefile" or "Makefile"
    if (!(*makefile_ref)) {
        if (0 == access("makefile", R_OK)) {
            *makefile_ref = "makefile";
        } else if (0 == access("Makefile", R_OK)) {
            *makefile_ref = "Makefile";
        }
    }
}

// Separate main file used for grading. Do not modify!
int main(int argc, char **argv) {
    char *makefile = NULL;
    size_t num_threads = 1;
    char **targets = NULL;
    parse_args(argc, argv, &makefile, &num_threads, &targets);
    // calls the student code
    parmake(makefile, num_threads, targets);
}
