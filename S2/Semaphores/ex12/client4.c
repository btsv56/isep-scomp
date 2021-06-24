/*
 * client4.c
 *
 *  Created on: 08/05/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "ticket.h"
#include <string.h>

int main() {
	sem_t *sm_seller,*sm_buyer,*sinc[2]; /* Controla a concorrencia dos filhos */;
	int fd,data_size=sizeof(ticket),i,n=0;
	char name[20];
	ticket *ticket;
	/* Abertura de zona de memoria partilhada */
	fd = shm_open("/shm_ex12", O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if (fd == -1) {
		perror("Erro no shm_open()\n");
		exit(1);
	}
	if (ftruncate(fd, data_size) == -1) {
		perror("Erro no ftruncate()\n");
		exit(1);
	}
	ticket = mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ticket == MAP_FAILED) {
		perror("Erro no mmap()\n");
		exit(1);
	}
	/* Criação dos semaforos */
	if ((sm_seller = sem_open("sm_seller",O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("Erro no sm_open(seller)\n");
		exit(1);
	}
	if ((sm_buyer = sem_open("sm_buyer", O_EXCL, 0644, 0)) == SEM_FAILED) {
		perror("Erro no sm_open(buyer\n");
		exit(1);
	}
	for(i=3;i<=4;i++) {
		sprintf(name,"sinc_%d",i);
		if((sinc[n]=sem_open(name,O_EXCL,0644,1))==SEM_FAILED) {
			perror("Erro no sm_open(sinc)\n");
			exit(1);
		}
		n++;
	}
	/* O primeiro client a chegar é o primeiro a ser atendido */
	if(sem_wait(sinc[0])==-1) { perror("Erro no sem_wait(sinc)\n"); exit(1);}
	/* Espera que o vendedor esteja pronto a atende lo */
	if(sem_wait(sm_buyer)==-1) {perror("Erro no sem_wait(sm_buyer)\n");exit(1);}
	sleep(rand()%10+1);
	printf("Client: %d Ticket: %d\n",getpid(),ticket->number);
	/*Informa o vendedor que pode pegar no bilhete seguinte */
	if(sem_post(sm_seller)==-1) {perror("Erro no sem_post(sm_seller\n");exit(1);}
	/* Informa o proximo cliente que pode ser atendido*/
	if(sem_post(sinc[1])==-1) {perror("Erro no sem_post(sinc)\n");exit(1);}
	/* closes the shared memory area */
	if (munmap(ticket, data_size) == -1) {
		perror("Erro no munmap()\n");
		exit(1);
	}
	if (close(fd)==-1) {
		perror("erro no close(fd)\n");
		exit(1);
	}
	exit(0);

}


