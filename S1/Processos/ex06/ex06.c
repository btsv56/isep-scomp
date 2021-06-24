#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Revised by:
 *          - Bruno Ribeiro - 1180573
 *          - Bruno Veiga   - 1180712
 *          - Pedro Brandao - 1180715
 *          - João Leal - 1180723
 */

void maina() {
    int i;

    for (i=0;i<4;i++) {
        if (fork()==0) {
            printf("going to sleep\n");
            sleep(1);
        }
    }

    printf("This is the end.\n");
}

void mainb() {
    int i;

    for (i=0;i<4;i++) {
        if (fork()==0) {
            printf("going to sleep\n");
            sleep(1);
            break;
        }
    }

    printf("This is the end.\n");
}

void mainc() {
    int i, s[4];
    pid_t p;

    for (i=0;i<4;i++) {
        p=fork();
        s[i]=(int) p;
        if (p==0) {
            printf("%d going to sleep\n", getpid());
            sleep(1);
            break;
        }
    }
    if (p!=0) {
        for (i=0;i<4;i++) {
            if (s[i]%2==0) {
                waitpid(p, NULL, 0);
            }
        }
    }
    printf("This is the end.\n");
}

void maind() {
    int i, s[4];
    pid_t p;

    for (i=0;i<4;i++) {
        p=fork();
        s[i]=(int) p;
        if (p==0) {
            printf("%d going to sleep (%d)\n", getpid(), i+1);
            sleep(1);
            break;
        }
    }

    if (p!=0) {
        for (i=0;i<4;i++) {
            if (s[i]%2==0) {
               waitpid(p, NULL, 0);
            }
        }
    }
    printf("This is the end.\n");
}

void main() {
    maind();
}
