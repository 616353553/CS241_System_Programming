/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/

#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
typedef struct _job_info {
    int id;
    /* Add whatever other bookkeeping you need into this struct. */
    double arrive_time;
    double remaining_time;
    double recent_run_time;
    double required_time;
    double start_time;
    int priority;
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;
// global variables to be print later
int num_process = 0;
double total_wait_time = 0;
double total_response_time = 0;
double total_turnaround_time = 0;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    job_info *info_a = ((job *) a)->metadata;
    job_info *info_b = ((job *) b)->metadata;
    return (info_a->arrive_time < info_b->arrive_time) ? -1 : 1;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    job_info *info_a = ((job *) a)->metadata;
    job_info *info_b = ((job *) b)->metadata;
    int result = info_a->priority - info_b->priority;
    return (result == 0) ? break_tie(a, b) : ((result < 0) ? -1 : 1);
}

int comparer_psrtf(const void *a, const void *b) {
    job_info *info_a = ((job *) a)->metadata;
    job_info *info_b = ((job *) b)->metadata;
    int result = info_a->remaining_time - info_b->remaining_time;
    return (result == 0) ? break_tie(a, b) : ((result < 0) ? -1 : 1);
}

int comparer_rr(const void *a, const void *b) {
    job_info *info_a = ((job *) a)->metadata;
    job_info *info_b = ((job *) b)->metadata;
    int result = info_a->recent_run_time - info_b->recent_run_time;
    return (result == 0) ? break_tie(a, b) : ((result < 0) ? -1 : 1);
}

int comparer_sjf(const void *a, const void *b) {
    job_info *info_a = ((job *) a)->metadata;
    job_info *info_b = ((job *) b)->metadata;
    int result = info_a->required_time - info_b->required_time;
    return (result == 0) ? break_tie(a, b) : ((result < 0) ? -1 : 1);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    job_info *info = malloc(sizeof(job_info));
    info->id = job_number;
    info->arrive_time = time;
    info->remaining_time = sched_data->running_time;
    info->recent_run_time = -1;
    info->required_time = sched_data->running_time;
    info->start_time = -1;
    info->priority = sched_data->priority;
    newjob->metadata = info;
    priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    if (job_evicted == NULL) {
        return priqueue_peek(&pqueue);
    }
    // set times for current job
    job_info *info = job_evicted->metadata;
    info->recent_run_time = time;
    info->remaining_time -= 1;
    if (info->start_time < 0) {
        info->start_time = time - 1;
    }
    if (pqueue_scheme == PPRI || pqueue_scheme == PSRTF || pqueue_scheme == RR) {
        // move curent job to the end of the pqueue
        job *curr_job = priqueue_poll(&pqueue);
        priqueue_offer(&pqueue, curr_job);
        return priqueue_peek(&pqueue);
    }
    return job_evicted;
}

void scheduler_job_finished(job *job_done, double time) {
    num_process += 1;
    job_info *info = job_done->metadata;
    total_wait_time += time - info->arrive_time - info->required_time;
    total_response_time += info->start_time - info->arrive_time;
    total_turnaround_time += time - info->arrive_time;
    free(info);
    priqueue_poll(&pqueue);
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    return total_wait_time/num_process;
}

double scheduler_average_turnaround_time() {
    return total_turnaround_time/num_process;
}

double scheduler_average_response_time() {
    return total_response_time/num_process;
}

void scheduler_show_queue() {
    printf("********* priority queue *********\n");
    for (int i = 0; i < priqueue_size(&pqueue); ++i) {
        job *curr = priqueue_poll(&pqueue);
        job_info* info = curr->metadata;
        printf("id: %d\n", info->id);
        printf("--priority: %d\n", info->priority);
        printf("--arrive time: %f\n", info->arrive_time);
        printf("--starttime: %f\n", info->start_time);
        printf("--required time: %f\n", info->required_time);
        printf("--recent run time: %f\n", info->recent_run_time);
        printf("--remaining time: %f\n", info->remaining_time);
        priqueue_offer(&pqueue, curr);
    }
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
