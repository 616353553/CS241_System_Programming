/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/

#include "libscheduler.h"
#include <stdio.h>

priqueue_t get_priqueue(void) {
    return pqueue;
}
scheme_t get_scheme(void) {
    return pqueue_scheme;
}
comparer_t get_comparison_func(void) {
    return comparision_func;
}

static int usage();

/* BEGIN TESTS */

static int test_comparer_fcfs(void);
static int test_comparer_sjf(void);
static int test_comparer_pri(void);
static int test_comparer_ppri(void);
static int test_comparer_psrtf(void);
static int test_rr(void);
static int test_start_up_rr(void);
static int test_start_up_fcfs(void);
static int test_new_job(void);
static int test_avg_wait_time(void);
static int test_avg_turn_time(void);
static int test_avg_resp_time(void);
/* END TESTS */

int main(int argc, char *argv[]) {
    if (argc == 1) {
        usage();
    }
    int test_number = atoi(argv[1]);
    switch (test_number) {
    default:
        usage();
    case 1:
        return test_new_job();
    case 2:
        return test_comparer_fcfs();
    case 3:
        return test_comparer_sjf();
    case 4:
        return test_comparer_pri();
    case 5:
        return test_comparer_ppri();
    case 6:
        return test_comparer_psrtf();
    case 7:
        return test_rr();
    case 8:
        return test_avg_wait_time();
    case 9:
        return test_avg_turn_time();
    case 10:
        return test_avg_resp_time();
    }
}

static int usage(void) {
    fprintf(stderr, "%s\n", "Needs valid test number.");
    exit(13);
}

// This function will make and schedule a job with a specified
// runtime, priority and arrival time
int job_no = 1; // Global job number
static job *make_and_schedule_new_job(double runtime, double priority,
                                      double arrival) {
    job *new_job = malloc(sizeof(job));
    scheduler_info schedule_data;
    schedule_data.running_time = runtime;
    schedule_data.priority = priority;
    scheduler_new_job(new_job, job_no, arrival, &schedule_data);
    job_no++;
    return new_job;
}

// test scheduler_new_job fills out job->metadata
static int test_new_job(void) {
    scheduler_start_up(FCFS);

    job *j = make_and_schedule_new_job(12, 3, 1);
    assert(j->metadata);

    free(j);

    return 0;
}

static int test_comparer_fcfs(void) {
    scheduler_start_up(FCFS);

    // jobs arrival order: j1 < j2

    // schedule job1 at time 1
    job *j1 = make_and_schedule_new_job(10, 1, 1);

    // schedule job2 at time 3
    job *j2 = make_and_schedule_new_job(10, 1, 3);
    // check comparer result
    int result_schedule_first = comparer_fcfs(j1, j2);
    assert(result_schedule_first == -1);
    int result_schedule_second = comparer_fcfs(j2, j1);
    assert(result_schedule_second == 1);

    free(j1);
    free(j2);

    return 0;
}

static int test_comparer_sjf(void) {
    scheduler_start_up(SJF);

    // jobs running time order: j1 < j2 == j3

    // schedule job1
    job *j1 = make_and_schedule_new_job(1, 1, 1);

    // schedule job2
    job *j2 = make_and_schedule_new_job(3, 1, 2);

    // schedule job3
    job *j3 = make_and_schedule_new_job(3, 1, 3);

    // check comparer result
    int result_schedule_first = comparer_sjf(j1, j2);
    assert(result_schedule_first == -1);
    int result_schedule_second = comparer_sjf(j2, j1);
    assert(result_schedule_second == 1);
    int result_schedule_brokable_tie = comparer_sjf(j2, j3);
    assert(result_schedule_brokable_tie ==
           -1); // j2 has an earlier arrival time

    free(j1);
    free(j2);
    free(j3);

    return 0;
}

static int test_comparer_pri(void) {
    scheduler_start_up(PRI);

    // jobs priority order: j1 < j2 == j3

    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 1, 0);

    // schedule job2
    job *j2 = make_and_schedule_new_job(10, 2, 1);

    // schedule job3
    job *j3 = make_and_schedule_new_job(10, 2, 2);

    // check comparer result
    int result_schedule_first = comparer_pri(j1, j2);
    assert(result_schedule_first == -1);
    int result_schedule_second = comparer_pri(j2, j1);
    assert(result_schedule_second == 1);
    int result_schedule_brokable_tie = comparer_pri(j2, j3);
    assert(result_schedule_brokable_tie ==
           -1); // j2 has an earlier arrival time

    free(j1);
    free(j2);
    free(j3);

    return 0;
}

static int test_comparer_ppri(void) {
    scheduler_start_up(PPRI);

    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 2, 0);

    // check j1 should run next
    job *j_to_run = scheduler_quantum_expired(NULL, 0);
    assert(j_to_run == j1);

    // schedule job2
    job *j2 = make_and_schedule_new_job(10, 1, 1);

    // check j2 should run next
    j_to_run = scheduler_quantum_expired(j_to_run, 1);
    assert(j_to_run == j2);

    // schedule job3
    job *j3 = make_and_schedule_new_job(10, 1, 2);

    // check j2 should still run next, because it arrives earlier than j3
    j_to_run = scheduler_quantum_expired(j_to_run, 2);
    assert(j_to_run == j2);

    free(j1);
    free(j2);
    free(j3);

    return 0;
}

static int test_comparer_psrtf(void) {
    scheduler_start_up(PSRTF);

    // jobs remaining time (at time 1) order: j1 == j3 > j2 == j4

    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 1, 0);
    scheduler_quantum_expired(NULL, 0);

    // schedule job2; now job1 has remaining_time (updated to) 9, job2 has
    // remaining_time 3
    job *j2 = make_and_schedule_new_job(3, 1, 1);

    // schedule job3
    job *j3 = make_and_schedule_new_job(9, 1, 2);

    scheduler_quantum_expired(j1, 1);

    // check comparer result
    int result_schedule_first = comparer_psrtf(j1, j2);
    assert(result_schedule_first == 1);
    int result_schedule_second = comparer_psrtf(j2, j1);
    assert(result_schedule_second == -1);
    int result_schedule_brokable_tie =
        comparer_psrtf(j1, j3); // they both have remaining_time 9
    assert(result_schedule_brokable_tie ==
           -1); // j1 has an earlier arrival time

    free(j1);
    free(j2);
    free(j3);

    return 0;
}

static int test_rr(void) {
    scheduler_start_up(RR);

    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 1, 0);

    // check j1 should run next
    job *j_to_run = scheduler_quantum_expired(NULL, 0);
    assert(j_to_run == j1);

    // schedule job2
    job *j2 = make_and_schedule_new_job(1, 2, 1);

    // check j2 should run next
    j_to_run = scheduler_quantum_expired(j1, 1);
    assert(j_to_run == j2);

    // check j1 should run next; j2 has finished
    scheduler_job_finished(j2, 2);
    j_to_run = scheduler_quantum_expired(NULL, 2);
    assert(j_to_run == j1);

    // check j1 should run next, because it's the only job left
    j_to_run = scheduler_quantum_expired(j1, 2);
    assert(j_to_run == j1);

    free(j1);
    free(j2);
    return 0;
}

static int test_avg_wait_time(void) {
    scheduler_start_up(RR);
    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 1, 1);

    job *j2 = make_and_schedule_new_job(10, 1, 1);

    int count[2] = {0, 0};
    job *j = scheduler_quantum_expired(NULL, 1);
    for (int i = 2; i < 22; i++) {
        j = scheduler_quantum_expired(j, i);
        if (i == 20) {
            scheduler_job_finished(j1, i);
            if (j1 == j)
                j = NULL;
        }
    }
    scheduler_job_finished(j2, 22);
    assert(scheduler_average_waiting_time() == 10);

    job *j3 = make_and_schedule_new_job(10, 1, 24);

    j = scheduler_quantum_expired(NULL, 24);
    scheduler_quantum_expired(j, 34);
    scheduler_job_finished(j3, 34);

    assert(scheduler_average_waiting_time() == 20.0 / 3.0);
    free(j1);
    free(j2);
    free(j3);
    return 0;
}

static void psrtf_simulation(void) {
    scheduler_start_up(PSRTF);
    // schedule job1
    job *j1 = make_and_schedule_new_job(10, 1, 1);

    job *j2 = make_and_schedule_new_job(5, 1, 1);

    job *j = NULL;
    for (int i = 0; i <= 5; i++) {
        j = scheduler_quantum_expired(j, 1 + i);
    }
    scheduler_job_finished(j2, 6);

    j = NULL;
    for (int i = 0; i <= 10; i++) {
        j = scheduler_quantum_expired(j, i + 6);
    }
    scheduler_job_finished(j1, 17);
    free(j1);
    free(j2);
}

static int test_avg_turn_time(void) {
    psrtf_simulation();
    assert(scheduler_average_turnaround_time() == 21.0 / 2.0);
    return 0;
}

static int test_avg_resp_time(void) {
    psrtf_simulation();
    assert(scheduler_average_response_time() == 2.5);
    return 0;
}
