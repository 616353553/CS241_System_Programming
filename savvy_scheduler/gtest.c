/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/

#include "gthreads/gthreads.h"
#include "libscheduler.h"
#include <stdio.h>
#include <unistd.h>

typedef void (*runner_t)(void);

void timer(int t, int id) {
    printf("Timer start!\n");
    for (int i = 0; i < t * 10000; i++) {
        usleep(100);
        if (i % 1000 == 0) {
            printf("Thread %d says hi!\n", t);
        }
    }
}

int main(int argc, char **argv) {
    // TODO put tests here!
}
