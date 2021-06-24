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
#include "structs.h"

/**
 * This program deletes the shared memory and semaphores
 */

int main(){
    int fd, data_size=sizeof(Shm_struct);
    Shm_struct *sh_data;
	
    /* opens shared memory */
	fd = shm_open("/shm_ex10", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd==-1) { perror("Shared memory failure."); exit(1); }
	
	if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }
    
    sh_data = (Shm_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_data==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    /* closes shared memory */
    if (munmap(sh_data, 100) < 0) exit(1);

    if (shm_unlink("/shm_ex10") < 0) exit(1);

    /* opens shared memory again */
    fd = shm_open("/shm_ex10", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd==-1) { perror("Shared memory failure."); exit(1); }
	
	if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }
    
    sh_data = (Shm_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_data==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    sh_data->count=0;
    sh_data->readcount=0;
    sh_data->writercount=0;

    printf("Memory cleared.\n");

    return 0;
}