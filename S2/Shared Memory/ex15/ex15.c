/*
 * ex15.c
 *
 *  Created on: 17/04/2020
 *      Author: Pedro Brandao
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 *  Revised by: Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 10
#define REPETITIONS 3

int main() {
	int fd[2], i, j, cont = 0;
	int numeros[SIZE];
	pid_t p;
	
	if(pipe(fd) == -1){
		perror("Pipe Creation Failed");
		exit(1);
	}
	

	/* Criação de  um processo filho */
	p = fork();
	if (p == -1) {
		perror("Erro no fork\n");
		exit(1);
	}

	if (p > 0) {
		if(close(fd[0])==-1) {perror("Erro no close1\n");exit(1);}
		for ( i=0; i<REPETITIONS; i++) {
			for( j=0 ; j < SIZE; j++){
				numeros[j] = cont++;
			}
			if(write(fd[1], numeros, sizeof(numeros))==-1) {perror("Erro no write\n");exit(1);}
		}
		if(close(fd[1])==-1) {perror("Erro no close2\n");exit(1);}
	}
	if(p == 0){
		if(close(fd[1])==-1) {perror("Erro no close3\n");exit(1);}
		for ( i=0; i<REPETITIONS; i++) {
			if(read(fd[0], numeros, sizeof(numeros))==-1) {perror("Erro no read\n");exit(1);}
			for(j=0; j < SIZE; j++){
				printf("Consumer: %d\n", numeros[j]);
			}
		}
		if(close(fd[0])==-1) {perror("Erro no close4\n");exit(1);}
		exit(0);
	}

	exit(0);
}

