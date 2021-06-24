/*
 * ex05.c
 *
 *  Created on: 04/03/2020
 *      Author: isep
 *      Revised by:
 *              - Bruno Ribeiro - 1180573
 *              - Bruno Veiga   - 1180712
 *          	- Pedro Brandao - 1180715
 *              - João Leal - 1180723
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t p;
    int status;

    p = fork();
    if (p == 0) {
        sleep(1);
        exit(1);
    }

    p = fork();
    if (p == 0) {
        sleep(2);
        exit(2);
    }

    p = wait(&status);
    printf("%d: status: %d\n", p, WEXITSTATUS(status));
    p = wait(&status);
    printf("%d: status: %d\n", p, WEXITSTATUS(status));

    return 0;
}


