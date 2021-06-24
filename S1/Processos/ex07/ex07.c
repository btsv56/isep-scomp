/*
 * ex07.c
 *
 *  Created on: 04/03/2020
 *      Author: isep
 *      Revised by:
 *              - Bruno Ribeiro - 1180573
 *              - Bruno Veiga   - 1180712
 *        	    - Pedro Brandao - 1180715
 *              - João Leal - 1180723
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>

#define ARRAY_SIZE 1000

void main() {
    int numbers[ARRAY_SIZE]; /* array to lookup */
    int ponto_partida;
    int n; /* the number to find */
    time_t t; /* needed to init. the random number generator (RNG)*/
    int i;
    pid_t p;
    int count = 0;
    int status;

    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned) time(&t));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++){
        numbers[i] = rand() % 10000;
    }

    /* initialize n */
    n = rand() % 10000;

    count = 0;

    p = fork();
    if (p == 0) {
        ponto_partida = 0;
    } else {
        ponto_partida = ARRAY_SIZE / 2;
    }
    for (i = 0; i < ARRAY_SIZE / 2; i++) {
        if (numbers[ponto_partida + i] == n) {
            count++;
        }
    }
    if (p == 0) {
        exit(count);
    } else if (p < 0){
        wait(&status);
        printf("n was found %d times.\n", count + WEXITSTATUS(status));
        exit(0);
    } else {
        perror("Child not created.");
        exit(1);
    }

}




