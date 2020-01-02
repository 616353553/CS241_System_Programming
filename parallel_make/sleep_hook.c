/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds) {
    (void)seconds;
    _exit(123);
}

int usleep(useconds_t usec) {
    (void)usec;
    _exit(123);
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
    (void)sem;
    (void)abs_timeout;
    _exit(123);
}

unsigned int alarm(unsigned int seconds) {
    (void)seconds;
    _exit(123);
}
