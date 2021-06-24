/*
 * ex04.c
 *
 *  Created on: 21/02/2020
 *      Author: isep
 *      Revised by:
 *          - Bruno Ribeiro - 1180573
 *          - Bruno Veiga   - 1180712
 *          - Pedro Brandao - 1180715
 * 			- João Leal - 1180723
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	int a=0, b, c, d;
	b = (int) fork();
	c = (int) getpid(); /* getpid(), getppid(): unistd.h*/
	d = (int) getppid();
	a = a + 5;
 	printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);
}

