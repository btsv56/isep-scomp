/**
 * Author: João Leal©
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

void buy_beer() {
    printf("They only had Sagres :(\n");
}

void buy_chips() {
    printf("Lays acquired.\n");
}

void eat_and_drink(int i) {
    if (i) printf("Yum\n");
    else printf("Luv chips, luv beer, simple as\n");
}

int main() {
    int i;
    char name[20];
    pid_t pid;
    sem_t *sems[2];

    /* opens semaphores */
    for (i=0;i<2;i++) {
        sprintf(name, "number%d", i+1);
        if ((sems[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    }

    /* creates child */
    if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
        
    if (pid==0) {
        buy_beer(); /* buys beer, increments the first semaphore and waits for the second */
        if (sem_post(sems[0])==-1) { perror("Semaphore 'post' failure."); exit(1); }
        if (sem_wait(sems[1])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
        eat_and_drink(0);
        exit(0);
    } else {
        buy_chips(); /* buys chips, increments the second semaphore and waits for the first */
        if (sem_post(sems[1])==-1) { perror("Semaphore 'post' failure."); exit(1); }
        if (sem_wait(sems[0])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
        eat_and_drink(1);
    }

    wait(NULL);
    /* closes semaphores */
    for (i = 0; i < 2; i++){
		sprintf(name, "number%d", i+1);
		sem_unlink(name);
	}

    return 0;
}

