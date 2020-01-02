/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

/**
 * Sleep / timed wait module that causes student code to fail if it uses
 * any of these functions.
 *
 * Compile:
 * 	gcc -pthread -ldl -c sleep_hook.c
 * 	gcc -pthread -ldl sleep_hook.o main.c -o main
 */
#ifndef _SLEEP_HOOK_H_
#define _SLEEP_HOOK_H_

unsigned int sleep(unsigned int seconds);

unsigned int usleep(useconds_t usec);

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

unsigned int alarm(unsigned seconds);

#endif /* _SLEEP_HOOK_H_ */
