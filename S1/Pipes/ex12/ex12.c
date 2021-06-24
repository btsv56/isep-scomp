/*
 * ex12.c
 *
 *  Created on: 17/03/2020
 *      Author: 1180715
 *      Revised by:
 *      	- Bruno Veiga - 1180712
 * 			- João Leal - 1180723
 * 			- Bruno Ribeiro - 1180573
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 255
#define READERS 5

/* structure that has the information of a product, barcode and name*/
struct product{
	int barcode;
	char name[BUFFER_SIZE] ;
};

/* structure used to tell the parent which pid read a specific barcode */
struct read_barcode{
	int barcode;
	int pid;
};

void assign_products(struct product *list){
	struct product cookies;
	cookies.barcode = 0;
	strcpy(cookies.name,"Cookies");

	struct product meat;
	meat.barcode = 1;
	strcpy(meat.name,"Meat");

	struct product fish;
	fish.barcode = 2;
	strcpy(fish.name,"Fish");

	struct product oranges;
	oranges.barcode = 3;
	strcpy(oranges.name,"Orange");

	struct product cereals;
	cereals.barcode = 4;
	strcpy(cereals.name,"Cereals");

	list[0] = cookies;
	list[1] = meat;
	list[2] = fish;
	list[3] = oranges;
	list[4] = cereals;
}

/* this function is used to simulate the reading of a barcode */
void child_process(int *fd, int *fd_u, struct read_barcode r_barcode){
	srand(time(NULL) % getpid());

	/*simulates the reading of a barcode by randomly generating one and assigns
	the pid and the barcode to the struct passed as parameter*/
	int barcode = rand() % READERS;
	r_barcode.barcode = barcode;
	r_barcode.pid = getpid();
	/* name of the product passed by the pipe */
	char name[BUFFER_SIZE];

	printf("child %d read the barcode %d\n", getpid(), barcode);

	close(fd[0]);
	close(fd_u[1]);

	/* passes the structure to the parent process */
	write(fd[1], &r_barcode, sizeof(r_barcode));

	/* reads the name of the barcode read before */
	read(fd_u[0], name, BUFFER_SIZE * sizeof(char));

	printf("barcode %d, is correspondent to %s (pid:%d\n", barcode, name, getpid());

	close(fd[1]);
	close(fd_u[0]);
}

int main(){

	struct read_barcode r_barcode;
	pid_t p[READERS];
	int fd[2];
	int fd_u[READERS][2];
	int i, j;

	struct product prod_list[5];
	/* creates a product "data base" */
	assign_products(prod_list);

	/* pipe shared by all processes */
	if(pipe(fd) == -1){
		perror("error creating pipe\n");
		exit(-1);
	}

	/* creates n processes simulating the barcode readers */
	for(i = 0; i < READERS; i++){
		/* pipe exclusive to the reader */
		if(pipe(fd_u[i]) == -1){
			perror("error creating pipe\n");
			exit(-1);
		}

		p[i] = fork();
		if(p[i] == -1){
			perror("error creating fork\n");
			exit(-1);
		}
		if(p[i] == 0){
			child_process(fd, fd_u[i], r_barcode);
			exit(0);
		}
	}


	char name[BUFFER_SIZE];

	/* closes the extremities of the pipes that wont be used */
	close(fd[1]);

	for(i = 0; i < READERS; i++){
		close(fd_u[i][0]);
	}

	/* reads the barcodes from the child processes and searches for the product in the DB */
	for(i = 0; i < READERS; i++){
		read(fd[0], &r_barcode, sizeof(r_barcode));

		int index;
		for(j = 0; j < READERS; j++){
			if(p[j] == r_barcode.pid){
				index = j;
			}
		}
		/* searches for the product in the DB */
		for(j = 0; j < READERS; j++){
			if(prod_list[j].barcode == r_barcode.barcode) {
				strcpy(name, prod_list[j].name);
			}
		}
		/* passes the name of the product via pipe to the child using it's assigned file descriptor */
		write(fd_u[index][1], name, BUFFER_SIZE * sizeof(char));
	}

	close(fd[0]);
	for(i = 0; i < READERS; i++){
		close(fd_u[i][1]);
	}

	return 0;
}
