#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Revised by:
 *          - Bruno Ribeiro - 1180573
 *          - Pedro Brandao - 1180715
 * 			- João Leal - 1180723
 * 			- Bruno Veiga- 1180712
 */

int main() {
	pid_t p;
	if (fork() == 0) {
		printf("PID = %d\n", getpid());
		exit(0);
	}

	if ((p=fork()) == 0) {
		printf("PID = %d\n", getpid());
		exit(0);
	}

	printf("Parent PID = %d\n", getpid());

	printf("Waiting... (for PID=%d)\n",p);
	waitpid(p, NULL, 0);

	printf("Enter Loop...\n");
	while (1); /* Infinite loop */
}
