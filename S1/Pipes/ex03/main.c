/*
 * Author: Bruno Ribeiro - 1180573
 * Revised by:
 * 		- Bruno Veiga -1180712
 *      - João Leal - 1180723
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    char s1[15];
    int status;
    pid_t p;

    /* creates pipe */
    if (pipe(fd) == -1){
        perror("Pipe failed");
        exit(1);
    }

    /* creates fork */
    p = fork();
    if (p == -1){
        perror("Fork failed");
        exit(1);
    }

    /* PARENT */
    if (p > 0) {

        close(fd[0]);

        strcpy(s1,"Hello World");
        write(fd[1], s1, sizeof(s1)); /* writes Hello World */

        strcpy(s1, "Goodbye!"); /* writes Goodbye */
        write(fd[1], s1, sizeof(s1));

        close(fd[1]);

        wait(&status); /* waits for child process */

        if (WIFEXITED(status)){
            printf("PID: %d\nExit value: %d\n\n", p, WEXITSTATUS(status)); /* prints result*/
        } else {
            perror("Error in child process... Exiting program.");
            exit(1);
        }

    /* CHILD */
    } else if (p == 0) {

        close(fd[1]);

        read(fd[0], s1, sizeof(s1));
        printf("%s\n", s1);

        read(fd[0], s1, sizeof(s1));
        printf("%s\n", s1);

        close(fd[0]);
        exit(0);
    }
    return 0;
}
