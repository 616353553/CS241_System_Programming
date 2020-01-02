/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *quote_A;
static char *quote_B;

// Create some Semaphores!

static void *modifyB_printA();
static void *modifyA_printB();

int main(int argc, char **argv) {
    // Initialize your semaphores

    quote_A = strdup("J!epo(u!lopx!ipx!nboz!pg!zpv!ibwf!fwfs!nfu!Ejkltusb-!cvu!"
                     "zpv!qspcbcmz!lopx!uibu!bssphbodf!jo!dpnqvufs!tdjfodf!jt!"
                     "nfbtvsfe!jo!obop.Ejkltusbt/!.!Bmbo!Lbz");
    quote_B = strdup("Uif!rvftujpo!pg!xifuifs!dpnqvufst!dbo!uijol!jt!mjlf!uif!"
                     "rvftujpo!pg!xifuifs!tvcnbsjoft!dbo!txjn/ぞ!.!Fethfs!X/"
                     "!Ejkltusb");

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, modifyA_printB, NULL);
    pthread_create(&tid2, NULL, modifyB_printA, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    free(quote_B);
    free(quote_A);

    // Destroy your semaphores

    return 0;
}

static void *modifyA_printB() {
    int i = 0;
    while (quote_A[i]) {
        usleep(rand() & 15); // randomized slowdowns
        quote_A[i++]--;
    }

    printf("quote_B:\t%s\n", quote_B);
    return NULL;
}

static void *modifyB_printA() {
    int i = 0;
    while (quote_B[i]) {
        usleep(rand() & 100); // randomized slowdowns
        quote_B[i++]--;
    }

    printf("quote_A:\t%s\n", quote_A);
    return NULL;
}
