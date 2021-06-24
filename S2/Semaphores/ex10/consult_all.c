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
 * This program shows all records available
 */

int main(){
	int fd, data_size = sizeof(Shm_struct);
	int i;
	sem_t *sems[4];
	Shm_struct *sh_data;
		
    /* opens shared memory */
	fd = shm_open("/shm_ex10", O_RDWR, S_IRUSR|S_IWUSR);
	if (fd==-1) { perror("Shared memory failure."); exit(1); }
	
	if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }
    
    sh_data=(Shm_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_data==MAP_FAILED) { perror("Mmap failure."); exit(1); }

	/* opens semaphores */
	if ((sems[0] = sem_open("mute1", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    if ((sems[1] = sem_open("mute3", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    if ((sems[2] = sem_open("writer", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    if ((sems[3] = sem_open("reader", O_RDWR, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }

	/* semaphores make sure it's safe to read */
    if (sem_wait(sems[1])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    if (sem_wait(sems[3])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    if (sem_wait(sems[0])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    sh_data->readcount++;
    if (sh_data->readcount==1) {
        if (sem_wait(sems[2])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    }
    if (sem_post(sems[0])==-1) { perror("Semaphore 'post' failure."); exit(1); }
    if (sem_post(sems[3])==-1) { perror("Semaphore 'post' failure."); exit(1); }
    if (sem_post(sems[1])==-1) { perror("Semaphore 'post' failure."); exit(1); }
	
    /* prints every record available */
	if (sh_data->count!=0) {
		printf("Showing all records:\n");
		for(i=0;i<sh_data->count;i++){
			printf("\nID: %d\n",sh_data->records[i].id);
			printf("Name: %s\n",sh_data->records[i].name);
			printf("Address: %s\n",sh_data->records[i].address);
		}
	} else printf("There are no records.\n");

	if (sem_wait(sems[0])==-1) { perror("Semaphore 'wait' failure."); exit(1); }
    sh_data->readcount--;
    if (sh_data->readcount==0) {
        if (sem_post(sems[2])==-1) { perror("Semaphore 'post' failure."); exit(1); }
    }
    if (sem_post(sems[0])==-1) { perror("Semaphore 'post' failure."); exit(1); }

	if(munmap(sh_data, data_size)==-1) { perror("Munmap failure."); exit(1); }
    close(fd);
	
	return 0;
}