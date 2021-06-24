/*
 * ex08.c
 *
 *  Created on: 16/03/2020
 *      Author: 1180715
 *
 *  Revised by:
 *  	- Bruno Veiga - 1180712
 * 		- João Leal -1180723
 * 		- Bruno Ribeiro - 1180573
 */

#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NUM_CHILDS 10
#define BUFFER_SIZE 255

/* structure used to communicate with the child process */
struct input{
	int roundNumber;
	char message[BUFFER_SIZE];
};

int main(){

	int fd[2];
	int i, status;
	pid_t pids[NUM_CHILDS];
	char msg[] = "Win";
	struct input message[NUM_CHILDS];

	if(pipe(fd) == -1) {
		perror("error creating pipe\n");
		exit(1);
	}

	/* creates the 10 processes */
	for(i=0; i < 10; i++){
		pids[i] = fork();
		if(pids[i] == -1){
			perror("error creating child\n");
		}
		if(pids[i] == 0){ 					/* in the child process is created a structure and the message is read */
			struct input msg;
			close(fd[1]);
			read(fd[0], &msg, sizeof(msg));
			close(fd[0]);
			printf("%s!\nI got %d. My PID is: %d\n",msg.message, msg.roundNumber, getpid());
			exit(msg.roundNumber);
		}
	}

	close(fd[0]);

	/* every 2 seconds a message is written to the pipe */
	for(i = 0; i < NUM_CHILDS; i++){
		message[i].roundNumber = i+1;
		strcpy(message[i].message, msg);
		write(fd[1], &message[i],sizeof(message[i]));
		sleep(2);
	}

	close(fd[1]);

	/* waits for all the child processes printing the pid and the placement in the game */
	for(i = 0; i < 10; i++){
		waitpid(pids[i], &status, 0);
		if(WIFEXITED(status)){
			printf("PID: %d ended in %d\n", pids[i], WEXITSTATUS(status));
		}
	}

	return 0;
}


