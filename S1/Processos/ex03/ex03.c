#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Revised by:
 *          - Bruno Ribeiro - 1180573
 *          - Bruno Veiga   - 1180712
 *          - Pedro Brandao - 1180715
 * 			- João Leal - 1180723
 */

int main() {
	fork(); fork(); fork();
	printf("SCOMP!!!!!\n");
	return 0;
}


