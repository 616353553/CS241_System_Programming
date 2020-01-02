/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/
#include "format.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if(argc < 2) {
        print_time_usage();
    }
    pid_t pid = fork();
    if (pid == -1) {
        print_fork_failed();
    } else if (pid == 0) {
        execvp(argv[1], argv + 1);
        print_exec_failed();
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end);
            display_results(argv, (end.tv_nsec - start.tv_nsec)/1e9 + (end.tv_sec - start.tv_sec));
        }
    }
    return 0;
}