#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Revised by:
 *      - Bruno Ribeiro - 1180573
 * 	    - Pedro Brandao - 1180715
 * 		- João Leal - 1180723
 * 		- Bruno Veiga - 1180712
 */

int main() {
	int x = 1;
	pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
	if (p == 0) {
		x = x+1;
		printf("1. x = %d\n", x);
	} else {
		x = x-1;
		printf("2. x = %d\n", x);
	}
	printf("3. %d; x = %d\n", p, x);

	return 0;
}


