/**
*  Lab
* CS 241 - Fall 2018
*/

#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void close_pipes(int*, int);
void wait_all(pid_t*, int, char*);

int main(int argc, char **argv) {
    if (argc != 6) {
        exit(1);
    }
    // save/parse arguments
    char *input_file = argv[1], 
        *output_file = argv[2], 
        *mapper = argv[3],
        *reducer = argv[4],
        *num_mappers = argv[5];
    int mapper_count = atoi(num_mappers);
    // create pids for splitter and mapper
    pid_t splitter_pids[mapper_count];
    pid_t mapper_pids[mapper_count];
    // Create an input pipe for each mapper.
    int mapper_pipes[mapper_count * 2];
    for (int i = 0; i < mapper_count * 2; i += 2) {
        pipe(mapper_pipes + i);
    }
    // Create one input pipe for the reducer.
    int reducer_pipe[2];
	pipe(reducer_pipe);
    // Start a splitter process for each mapper.
    for (int i = 0; i < mapper_count; ++i) {
        splitter_pids[i] = fork();
        if (splitter_pids[i] == -1) {
            exit (1);
        } else if (splitter_pids[i] == 0) {
            // child process
            int result = dup2(mapper_pipes[2 * i + 1], 1);
            if (result == -1) {
                exit(1);
            }
            // close all pipes
            close_pipes(mapper_pipes, mapper_count * 2);
            close_pipes(reducer_pipe, 2);
            // parse integer i to string
            char i_str[20];
            sprintf(i_str, "%d", i);
            if (execl("splitter", "splitter", input_file, num_mappers, i_str, NULL) == -1) {
                exit(1);
            }
        }
    }
    // Start all the mapper processes.
    for (int i = 0; i < mapper_count; ++i) {
        mapper_pids[i] = fork();
        if (mapper_pids[i] == -1) {
            exit(1);
        } else if (mapper_pids[i] == 0) {
            int result_0 = dup2(mapper_pipes[i * 2], 0);
            int result_1 = dup2(reducer_pipe[1], 1);
            if (result_0 == -1 || result_1 == -1) {
                exit(1);
            }
            // close all pipes
            close_pipes(mapper_pipes, mapper_count * 2);
            close_pipes(reducer_pipe, 2);
            if (execl(mapper, mapper, NULL) == -1) {
                exit(1);
            }
        }
    }
    // Start the reducer process.
    pid_t reducer_pid = fork();
    if (reducer_pid == -1) {
        exit(1);
    } else if (reducer_pid > 0) {
        close_pipes(mapper_pipes, mapper_count * 2);
        close_pipes(reducer_pipe, 2);
    } else {
        // Open the output file.
        FILE* output_fp = freopen(output_file, "w+", stdout);
        if (output_fp == NULL) {
            exit(1);
        }
        int result = dup2(reducer_pipe[0], 0);
        if (result == -1) {
            exit(1);
        }
        close_pipes(mapper_pipes, mapper_count * 2);
        close_pipes(reducer_pipe, 2);
        if (execl(reducer, reducer, NULL) == -1) {
            exit(1);
        }
    }
    // Wait for the reducer to finish.
    wait_all(splitter_pids, mapper_count, "splitter");
    wait_all(mapper_pids, mapper_count, mapper);
    wait_all(&reducer_pid, 1, reducer);
    // Count the number of lines in the output file.
    FILE* output_fp = fopen(output_file, "r");
    int num_lines = 0;
    char buffer[500];
    while (fgets(buffer, 500, output_fp)) {
        ++num_lines;
    }
    fclose(output_fp);
    printf("%d lines in %s\n", num_lines, output_file);
    return 0;
}

void close_pipes(int *pipes, int size) {
    for (int i = 0; i < size; ++i)
        close(pipes[i]);
}

void wait_all(pid_t *pids, int num_pids, char *name) {
    int stat;
    for (int i = 0; i < num_pids; ++i) {
        waitpid(pids[i], &stat, 0);
        if (WIFEXITED(stat)) {
            int status = WEXITSTATUS(stat);
            // Print nonzero subprocess exit codes.
            if (status != 0) {
                printf("%s %d exited with status %d\n", name, i, status);
            }
        }
    }
}
