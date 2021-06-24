/*
 * Author: João Leal - 1108723
 * Revised by:
 * 		- Bruno Veiga - 1180712
 * 		- Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define INT_S 4

/* returns a random integer between [1-n] (altered for consistency) */
int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        if (n > RAND_MAX) return -1;

        int end = RAND_MAX / n;
        if (end <= 0) return -1;
        end *= n;

        int r;
        while ((r = rand()) >= end);
        return r % n + 1;
    }
}

int main() {
    int fd[2], fd1[2], credit=20;
    pid_t pid;

    /* creates two pipes, one that sends info from parent to child, another the other way around: from child to parent */
    if (pipe(fd)==-1 || pipe(fd1)==-1) {
        perror("Pipe error");
        return -1;
    }

    if ((pid=fork())==-1) {perror("Fork failed"); return -1;}

    if (pid>0) {

        int hand=randint(5), cont=1, end=0, bet, i=0;

        close(fd[0]);
        close(fd1[1]);

        printf("Round %d\n", ++i);
        /* while child has credits */
        while (credit>0) {
            /* sends confirmation flag and reads the bet */
            write(fd[1], &cont, INT_S);
            read(fd1[0], &bet, INT_S);
            printf("Hand: %d, Bet: %d\n", hand, bet);
            if (bet==hand) { /* if bet is correct, adds 10 to credits and randomizes another int */
                credit+=10; 
                hand=randint(5);
                printf("Correct!\n\nRound %d\n", ++i);    
            } else {credit-=5;}
            write(fd[1], &credit, INT_S);
        }
        /* when the child runs out of credits, sends 0 */
        write(fd[1], &end, INT_S);

        close(fd[1]);
        close(fd1[0]);
    } else {
        int conf=1, bet;

        close(fd[1]);
        close(fd1[0]);

        /* sets new rand() seed */
        srand(time(NULL));
        
        /* reads confirmation flag */
        read(fd[0], &conf, INT_S);
        while (conf) {
            /* sends bet and reads resulting credits, reads flag again */
            bet=randint(5);
            write(fd1[1], &bet, INT_S);
            read(fd[0], &credit, INT_S);
            printf("Credits: %d\n", credit);
            read(fd[0], &conf, INT_S);
        }
        printf("Child has run out of credits!\n");
        close(fd1[1]);
        close(fd[0]);
    }

    return 0;
}
