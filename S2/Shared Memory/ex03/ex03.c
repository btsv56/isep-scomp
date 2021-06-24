/*
 * ex03.c
 *
 *  Created on: 28/03/2020
 *      Author: sami0l
 * 
 *  Revised by: Bruno Veiga - 1180712
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

#define ARRAY_SIZE 10000
#define shm_name "ex03"

//array with the number and string
typedef struct{
	int number;
	char string[20];
} shared_array;

//shared type between the shared memory and pipe
typedef struct{
	shared_array array[ARRAY_SIZE];
	int new_data;
} shared_data_type;

//fills a the shared_data_type with numbers and "ISEP - SCOMP 2020"
void fill_vec(shared_data_type *array){
	int i;
	for(i = 0; i < ARRAY_SIZE; i++){
		array->array[i].number = i;
		strcpy(array->array[i].string, "ISEP - SCOMP 2020");
	}
}

//transfers the shared_data_type to another process using a pipe as means of communication
double pipe_time(shared_data_type original){
	int fd[2];
	int data_size = sizeof(shared_data_type);
	int status;
	pid_t p;

	clock_t start_time, end_time;

	if(pipe(fd) == -1){
		perror("pipe creation failed");
		exit(1);
	}

	p = fork();
	if(p < 0){
		perror("fork failed");
		exit(1);
	}
    start_time = clock();

	//father process
	if(p > 0){

		//closes the read fd, and writes to the pipe, and closes the write fd
		close(fd[0]);
		write(fd[1], &original, data_size);
		close(fd[1]);

		//waits for the child process and returns the time taken
		wait(&status);
		if(WIFEXITED(status)){
			return WEXITSTATUS(status - start_time);
		}
	}

	//child process
	if(p == 0){
		shared_data_type copy;
		//closes the write fd, reads from the pipe and closes the read fd
		close(fd[1]);
		read(fd[0], &copy, data_size);
		close(fd[0]);

		end_time = clock();
		//exits the child process with the end time as it's status
		exit(end_time);
	}

	return -1;
}

//transfers the shared_data_type to another process using the shared memory as means of communication
double shared_memory_time(shared_data_type original, shared_data_type copy){

	int status;
	pid_t p;
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *memory_shared;

	clock_t start_time, end_time;

	//creates and opens a shared memory area
	fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("shared memory creation failed\n");
		exit(1);
	}

	//defines the area size and initializes it to 0
	if (ftruncate(fd, data_size) == -1) {
		perror("size allocation failure.\n");
		exit(1);
	}

	//maps a shared memory object in the address space of this process
	memory_shared = (shared_data_type *) mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(memory_shared == MAP_FAILED){
		perror("mmap failure.");
		exit(1);
	}

	p = fork();

	if(p == -1){
		perror("fork failed");
		exit(1);
	}
    start_time = clock();

	//father process
	if(p > 0){

		//copies the array to the shared memory space
		memory_shared = &original;

		wait(&status);
		if(WIFEXITED(status)){
			return WEXITSTATUS(status - start_time);
		}

    }

	if(p == 0){
		//copies the shared memory to a shared_data_type in the child process
		copy = *memory_shared;
		end_time = clock();
        if (close(fd) == -1){
            perror("failed closing file descriptor");
            exit(1);
        }

        if (munmap(memory_shared, sizeof(memory_shared)) == -1){
            perror("failed ");
            exit(1);
        }

        if (shm_unlink(shm_name) == -1){
            perror("failed shm_unlink");
            exit(1);
        }
		exit(end_time);
	}

	return -1;
}


int main(int argc, char *argv[]){

	shared_data_type original, copy;

	fill_vec(&original);
	original.new_data = 1;

	printf("shared memory time: %.2f\n", shared_memory_time(original, copy));
	printf("pipe transfer time: %.2f\n", pipe_time(original));

	return 0;

}
