/**
 * Revised by:
 *	    - Bruno Veiga, 1180712
 *          - Pedro Brandao - 1180715
 *          - João Leal - 1180723
 *          - Bruno Veiga - 1180712
 */
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* Creates N child processes returning the order of its creation. */
int spawn_childs(int n){
    pid_t p; /* pid of child process */
    int i;

    /* creates n child processes that return it's order. Parent returns zero. */
    for (i = 1; i <= n; i++){

        p = fork();
        if (p == 0){
            return(i);
        } else if (p == -1){
            return -1;
        }

    }
    return(0);
}

int main() {
    int number_childs = 6,index,i,status;

    index = spawn_childs(number_childs);

    /* If there's a problem creating a process it will show an error and exit the program. */
    if (index != -1){

        /* Multiply by 2 using logic operators. */
        if (index != 0){
            exit(index << 1);
        }

        for (i = 0; i < number_childs; ++i) {
            wait(&status);
            if (WIFEXITED(status)){
                printf("%d\n", WEXITSTATUS(status));
            }
        }

        exit(0);

    } else {
        perror("Couldn't create all processes.");
        exit(1);
    }
}
