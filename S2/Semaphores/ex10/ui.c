/**
 * Author: João Leal©
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include "structs.h"

/**
 * This program serves as an user interface
 */
int main() {
    int fd, data_size=sizeof(Shm_struct);
    char input;
    sem_t *sems[5];
    Shm_struct *sh_data;

    /* opens shared memory */
    fd = shm_open("/shm_ex10", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    sh_data=(Shm_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_data==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    /* Initializes struct variables */
    sh_data->readcount=0;
    sh_data->writercount=0;

    /* opens semaphores */
    /* protects variable readcount */
    if ((sems[0] = sem_open("mute1", O_CREAT | O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); } 
    /* protects variable writercount */
    if ((sems[1] = sem_open("mute2", O_CREAT | O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    /* makes sure only one reader is trying to access records */
    if ((sems[2] = sem_open("mute3", O_CREAT | O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    /* writer semaphore */
    if ((sems[3] = sem_open("writer", O_CREAT | O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    /* reader semaphore */
    if ((sems[4] = sem_open("reader", O_CREAT | O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    
    do {
        printf("\n1 - Insert record\n2 - Consult record\n3 - Consult all records\n4 - Clear memory\n0 - Exit\n\n");
        scanf("%c%*c", &input);
        switch (input) {
        case '1':
            system("./insert");
            break;
        case '2':
            system("./consult");
            break;
        case '3':
            system("./consult_all");
            break;
        case '4':
            system("./clear_mem");
            break;
        case '0':
            printf("Closing program...\n");
            break;
        default:
            printf("Input not recognized\n");
            break;
        }
    } while (input!='0');

    /* Closes shared memeory */
    if(munmap(sh_data, data_size)==-1) { perror("Munmap failure."); exit(1); }
    close(fd);

    return 0;
}