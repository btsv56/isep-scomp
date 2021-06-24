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
 * Wrapper function for fgets
 */
char *fgets_wrapper(char *buffer, size_t buflen, FILE *fp) {
    if (fgets(buffer, buflen, fp) != 0) {
        buffer[strcspn(buffer, "\n")] = '\0';
        return buffer;
    }
    return 0;
}

/**
 * This program inserts a new record
 */
int main() {
    int fd, data_size=sizeof(Shm_struct), i, id, flag;
    sem_t *sems[3];
    Shm_struct *sh_data;
    User user;

    /* opens shared memory */
    fd = shm_open("/shm_ex10", O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    sh_data=(Shm_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_data==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    /* opens sempahores */
    if ((sems[0] = sem_open("mute2", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    if ((sems[1] = sem_open("writer", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    if ((sems[2] = sem_open("reader", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }

    if (sem_wait(sems[0])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    sh_data->writercount++;
    if (sh_data->writercount==1) {
        /* stops readers */
        if (sem_wait(sems[2])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    }
    if (sem_post(sems[0])==-1) { perror("Semaphore 'post' failure."); exit(1); }

    /* waits for other writers to finish */
    if (sem_wait(sems[1])==-1) { perror("Semaphore 'wait' failure."); exit(1); }

    /* if the record count is 100, no new records are able to be inserted */
    if (sh_data->count==100) { printf("No more space\n"); exit(1); }

    /* insertion begins */
    printf("Inserting new record\n\n");
	printf("Name:\n");
	fgets_wrapper(user.name, sizeof(user.name), stdin);

    /* makes sure id number is unique */
    do {
        printf("ID:\n");
        scanf("%d%*c", &id);
        flag=0;
        for(i=0; i<sh_data->count; i++){
            if(sh_data->records[i].id==id) {
                printf("\nID number already in use\n");
                flag=1;
                break;
            }
        }
    } while (flag);

    user.id=id;

	printf("Address:\n");
    fgets_wrapper(user.address, sizeof(user.address), stdin);

    /* adds user to records */
	sh_data->records[sh_data->count]=user;
    sh_data->count++;

    /* allows writers */
    if (sem_post(sems[1])==-1) { perror("Semaphore 'post' failure."); exit(1); }

    if (sem_wait(sems[0])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    sh_data->writercount--;
    if (sh_data->writercount==0) {
        /* allows readers */
        if (sem_post(sems[2])==-1) { perror("Semaphore 'post' failure."); exit(1); }
    }
    if (sem_post(sems[0])==-1) { perror("Semaphore 'post' failure."); exit(1); }

    if(munmap(sh_data, data_size)==-1) { perror("Munmap failure."); exit(1); }
    close(fd);

    printf("New record inserted successfully.\n");
    return 0;
}