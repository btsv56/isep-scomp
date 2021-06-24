/*
 * writer.c
 *
 *  Created on: 03/04/2020
 *      Author: sami0l
 * 	Revised by: Bruno Veiga - 1180712
 * 				Bruno Ribeiro - 1180573
 *  Revised by: João Leal - 1180723
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE 10
#define shm_name "/ex07"

typedef struct {
	int array[10];
	int new_data;
} shared_data_type;

int main(void){

	int i;
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *memory_shared;

	/*creates and opens a shared memory area*/
	fd = shm_open(shm_name, O_CREAT |O_EXCL| O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("shared memory creation failed\n");
		exit(1);
	}

	/*defines the area size and initializes it to 0*/
	if (ftruncate(fd, data_size) == -1) {
		perror("size allocation failure.\n");
		exit(1);
	}

	/*maps a shared memory object in the address space of this process*/
	memory_shared = (shared_data_type *) mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(memory_shared == MAP_FAILED){
		perror("mmap failure.");
		exit(1);
	}

	/* fills the array in the shared memory space with random numbers from 1 to 20 */
	srand((unsigned) time(NULL));
	for(i = 0; i < ARRAY_SIZE; i++){
		memory_shared->array[i] = rand() % 20+1;
	}
	/* activates the flag indicating that there is data in the shared memory */
	memory_shared->new_data = 1;

	/* unmaps the memory from the address space of the process */
	if(munmap(memory_shared, data_size) == -1){
		perror("munmap error\n");
		exit(1);
	}

	/* closes file descriptor */
	if(close(fd) == -1){
		perror("error closing file desc\n");
	}

	exit(0);
}
