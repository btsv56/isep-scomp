/**
 * Revised by: João Leal - 1180723
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

int main(){
    /*pid of the process*/
    pid_t pid;
    /*name of the semaphore*/
    const char *SEM_NAME = "ex05";
    /*initial value of the semaphore*/
    const int SEM_INIT_VALUE = 0;
    /*pointer of the semaphore*/
    sem_t *semaphore;

    /*Creates a semaphore*/
    if((semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, SEM_INIT_VALUE)) == SEM_FAILED) {
        perror("Semaphore Creation Failed!\n");
        exit(EXIT_FAILURE);
    }

    /*creates and validates a fork*/
    pid = fork();
    if(pid == -1){
        perror("Process Creation Failed!\n");
        exit(EXIT_FAILURE);
    }

    /*the child process prints the message. This message is
     * printed first since the father process is moved to the semaphore queue.
     * After printing the message the process is removed from the queue
     * and executed*/
    if(pid == 0){
        printf("I'm the Child!\n");
        if(sem_post(semaphore) == -1){
            perror("Error incrementing the semaphore!\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    /*the father process prints the message*/
    if(pid > 0){
        if(sem_wait(semaphore) == -1){
            perror("Error decrementing the semaphore!\n");
            exit(EXIT_FAILURE);
        }
        printf("I'm the Father!\n");
    }

    /*deletes the semaphore created*/
    if(sem_unlink(SEM_NAME) == -1){
        perror("Error deleting the semaphore!\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}