/*
 * ex02.c
 *
 *  Created on: 21/02/2020
 *      Author: sami0l
 *
 *  Revised by:
 *      - Bruno Ribeiro - 1180573
 * 		- João Leal - 1180723
 * 		- Bruno Veiga- 1180712
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {

	pid_t p = fork();

	if(p > 0){
		printf("I'm...\n");
	} else {
		printf("I'll never join you!\n");
		exit(0);
	}

	p = fork();
	if(p > 0){
		printf("The...\n");
	} else {
		printf("I'll never join you!\n");
		exit(0);
	}

	p = fork();
	if(p > 0){
		printf("Father!\n");
	} else {
		printf("I'll never join you!\n");
		exit(0);
	}

	return 0;
}
