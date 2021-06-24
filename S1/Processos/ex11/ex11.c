/*
 * ex11.c
 *
 *  Created on: 06/03/2020
 *      Author: isep
 *      Revised by:
 *              - Bruno Ribeiro - 1180573
 *          	- Pedro Brandao - 1180715
 * 				- João Leal - 1180723
 * 				- Bruno Veiga - 1180712
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {

	int numbers[1000], i, j, status, max, result[500];
	pid_t p;


	printf("Numbers: \n");


	for (i = 0; i < 1000; i++) {
		numbers[i] = rand() % 256; /*Initializes the array with random number between 0 and 255*/
	}

	for (i = 0; i < 5; i++) {
		p = fork(); /* Creates the 5 processes */

		if (p == -1) { /* Verifies if all the processes were created */
			perror("It's not possible to create the process");
			exit(1);
		}

		if (p == 0) {
			int tempMax;
			int temp;
			for (j = 0; j < 200; j++) {
				temp = numbers[j + (i * 200)];
				if (tempMax < temp) {
					tempMax = temp; /* Finds the maximum value */
				}
			}
			exit(tempMax);
		}
	}


	for (i = 0; i < 5; i++) {
		wait(&status);
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) > max) {
				max = WEXITSTATUS(status);
			}
		}
	}


	printf("Max value: %d\n", max);


	p = fork();

	if (p == 0) {
		i = 0; /* Decides where the child process begins in the array */
	} else {
		i = 500; /* Decides where the parent process begins in the array */
	}

	for (j = 0; j < 500; j++) {
		result[j] = ((int) numbers[i + j] / max) * 100; /* Both parent and child do the calculation in different parts of the array */
	}

	if (p == 0) {
		printf("Results:\n");
		for (i = 0; i < 500; i++) {
			printf("%d = %d\n",i, result[i]); /* Shows first 500 results */
		}
		exit(0); /* Terminates the child process after showing the 500 numbers of the array of results */
	} else {
		wait(&status);
		for (i = 0; i < 500; i++) {
			printf("%d = %d\n",i, result[i]); /* Shows the last 500 results */
		}
	}


	return 0;
}
