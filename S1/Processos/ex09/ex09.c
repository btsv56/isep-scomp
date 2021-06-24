/*
 * ex09.c
 *
 *  Created on: 28/02/2020
 *      Author: isep
 *	Revised by:
 *          - Pedro Brandao - 1180715
 * 			- João Leal - 1180723
 *          - Bruno Veiga - 1180712
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
//Creates 10 child processes that print 100 numbers each
void main() {
	pid_t p;
	int i,n,j;
	for (i = 0; i < 10; i++) {
		p = fork();
		if(p==-1) {
			perror("Couldn't create process");
			exit(1);
		}
		if (p == 0) {
			for (j= i*100+1; j <= (i+1)*100; j++) {
				printf("%d\n", j);
			}
			exit(0);
		}
	}
	for(i=0;i<10;i++) {
		wait(&n); //waits for the child process to continue
        if(!WIFEXITED(n)){
            perror("Child didn't exit well.");
            exit(1);
        }
    }
	exit(0);
}
