/*
 * ex04.c
 *
 *  Created on: 16/03/2020
 *      Author: 1180715
 *      Revised by:
 *      	Bruno Veiga-1180712
 * 			João Leal - 1180723
 *          Bruno Ribeiro - 1180573
 */

#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){

	int fd[2];
	pid_t p;
	int status;
	FILE *fptr;
	int num;

	/*creates the pipe */
	if(pipe(fd) == -1) {
		perror("pipe creation failed");
		exit(1);
	}

	/* opens the file with the text */
	if((fptr = fopen("file.txt","r")) == NULL){
		printf("error opening file");
		exit(1);
	}

	/* scans the content of the file */
	fscanf(fptr,"%d", &num);

	/* creates a fork */
	p = fork();
	if(p == -1){
		perror("process creation failed");
		exit(1);
	}

	/* the parent writes the content of the file to a pipe */
	if(p > 0){
		close(fd[0]);
		write(fd[1],&num,sizeof(num));
		close(fd[1]);
	} else { /*  the child reads the pipe and prints it */
		close(fd[1]);
		int nump;
		read(fd[0],&nump,sizeof(nump));
		printf("%d",nump);
		close(fd[0]);
		exit(0);
	}

	/* closes the file */
	fclose(fptr);

	/* waits for the child */
	wait(&status);

	exit(0);

}
