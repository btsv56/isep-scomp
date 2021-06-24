/**
 * Revised by: João Leal - 1180723
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){

    /*names of the semaphores*/
    const char SEM_NAME[2][20] = {"father_sem", "child_sem"};
    /*initial values of the semaphores*/
    const int SEM_INIT_VALUE[2] = {0,1};
    /*index of the father semaphore*/
    const int SEM_FATHER = 0;
    /*index of the child semaphore*/
    const int SEM_CHILD = 1;
    /*memory address of the semaphores*/
    sem_t *semaphore[2];
    /*counter of the times the message is printed*/
    int cont = 0;

    int i;

    /* Creates the semaphores for the two process*/
    for(i = 0; i < 2; i++){
        if((semaphore[i] = sem_open(SEM_NAME[i], O_CREAT | O_EXCL, 0644, SEM_INIT_VALUE[i])) == SEM_FAILED){
            perror("Error Creating Semaphores!\n");
            exit(EXIT_FAILURE);
        }
    }

    /*Creates a fork and validates it's creation*/
    pid_t pid = fork();
    if(pid == -1){
        perror("Process Creation Failed!\n");
        exit(EXIT_FAILURE);
    }

    /* By using the function sem_wait we move the process to the semaphore queue,
     * blocking the process. sem_post, on the other hand, removes the process from the
     * queue. This way, we ensure that the messages are printed in an
     * alternate way.*/
    while(cont < 7){
        if(pid == 0){
            if(sem_wait(semaphore[SEM_CHILD]) == -1){
                perror("Error decrementing the value of the semaphore\n");
                exit(EXIT_FAILURE);
            }
            printf("I'm the Child!\n");
            cont++;
            if(sem_post(semaphore[SEM_FATHER]) == -1) {
                perror("Error incrementing the value of the semaphore\n");
                exit(EXIT_FAILURE);
            }
        }
        if(pid > 0){
            if(sem_wait(semaphore[SEM_FATHER]) == -1){
                perror("Error decrementing the value of the semaphore\n");
                exit(EXIT_FAILURE);
            }
            printf("I'm the Father!\n");
            cont++;
            if(sem_post(semaphore[SEM_CHILD]) == -1) {
                perror("Error incrementing the value of the semaphore\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* finishes the child process*/
    if(pid == 0){
        exit(EXIT_SUCCESS);
    }
	
	/* Waits for the children to end */
	for(cont=0;cont<7;cont++) {
		wait(NULL);
	}

    /* deletes the semaphores created*/
    for(i = 0; i < 2; i++){
        if(sem_unlink(SEM_NAME[i]) == -1){
            perror("Error Deleting Semaphores!\n");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;

}