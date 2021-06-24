#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define ARRAY_SIZE 2000

/*
 * Revised by:
 *          - Bruno Ribeiro - 1180573
 *          - Pedro Brandao - 1180715
 * 			- João Leal - 1180723
 * 			- Bruno Veiga - 1180712
 */

void main() {
	int array[ARRAY_SIZE], n, status=1;
	pid_t p;
	int i,j;



	printf("Write a number:");
	scanf("%d", &n);



	/* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned) time(&t));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++){
        numbers[i] = rand() % 2001;
    }



	for(i=0;i<10;i++) {
		p = fork(); //Creates a child process
		if (p == 0) {
			for (j = i*200; j < 200*(i+1); j++) {
				if (array[j] == n) { // Verifies if the array has the number pretended
					exit(j-(200*i)); // Exits with relative position in the array
				}
			}
			exit(255); // Exit with the return value of 255 if the number doesn't exist in a relative part of the array
		}
		if (p == -1){
		    perror("Couldn't create child.");
		    exit(1);
		}
	}


	for(i=0;i<10;i++) {
		wait(&status); //Waits for the child processes to finish
		if(WIFEXITED(status) && WEXITSTATUS(status)!=255 ) {
			printf("%d\n",WEXITSTATUS(status)); // prints the valid indexes returned
		}
	}
	exit(0);
}
