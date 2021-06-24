/*
 * ex12.c
 *
 *  Created on: 24/04/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "ticket.h"
#include "sys/wait.h"
#include <string.h>

#define FILHOS 5

int main() {
	int fd,data_size=sizeof(ticket),i,n=0;
	pid_t p;
	sem_t *sm_seller /* Controls when the seller need a new ticket number*/ ,*sm_buyer /* Controls when the buyer can get the ticket number */;
	sem_t *sinc[10];
	char name[20];
	ticket *ticket;
	/* Abertura de zona de memoria partilhada */
	fd=shm_open("/shm_ex12",O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd==-1) {
		perror("Erro no shm_open()\n");
		exit(1);
	}
	if(ftruncate(fd,data_size)==-1) {
		perror("Erro no ftruncate()\n");
		exit(1);
	}
	ticket=mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(ticket == MAP_FAILED) {
		perror("Erro no mmap()\n");
		exit(1);
	}
	/* Criação dos semaforos */
	if((sm_seller=sem_open("sm_seller",O_CREAT|O_EXCL, 0644,1))==SEM_FAILED) {
		perror("Erro no sm_open(seller)\n");
		exit(1);
	}
	if((sm_buyer=sem_open("sm_buyer",O_CREAT|O_EXCL,0644,0))==SEM_FAILED) {
		perror("Erro no sm_open(buyer\n");
		exit(1);
	}
	if((sinc[0]=sem_open("sinc_0",O_CREAT|O_EXCL,0644,1))==SEM_FAILED) {
		perror("Erro no sm_open(sinc)\n");
		exit(1);
	}
	for(i=1;i<FILHOS;i++) {
		sprintf(name,"sinc_%d",i);
		if((sinc[i]=sem_open(name,O_CREAT|O_EXCL,0644,0))==SEM_FAILED) {
			perror("Erro no sm_open(sinc)\n");
			exit(1);
		}
	}
	/* Criação dos processos filho */
	for(i=0;i<FILHOS;i++) {
		p=fork();
		if(p==-1) {
			perror("Erro no fork()\n");
			exit(1);
		}
		if(p==0) {
			sprintf(name,"./client%d",i+1);
			printf("%s\n",name);
			if(execlp(name,name,NULL)==-1) {
				perror("Erro no execlp\n");
				exit(1);
			}
		}
	}

	/* Obtenção do número do ticket para o consumidor */
	while(n<FILHOS) {
		if(sem_wait(sm_seller)==-1) {perror("Erro no sm_wait(seller)\n");exit(1);}
		ticket->number++;
		if(sem_post(sm_buyer)==-1) { perror("Erro no sm_wait(buyer)\n");exit(1);}
		n++;
	}
	for(i=0;i<FILHOS;i++) {
		wait(NULL);
	}
	/*Remove a zona de memoria partilhada */
	if (munmap(ticket, data_size) == -1) {
		perror("Erro no munmap()\n");
		exit(1);
	}
	if(close(fd)) {
		perror("Erro no close(fd)\n");
		exit(1);
	}
	if (shm_unlink("/shm_ex12") == -1) {
		perror("erro no shm_unlink()\n");
		exit(1);
	}
	/* Elimina os semaforos */
	if(sem_unlink("sm_seller")==-1) {
		perror("Erro no sem_unlink(sem_seller)\n");
		exit(1);
	}
	if(sem_unlink("sm_buyer")==-1) {
		perror("Erro no sem_unlink(sem_buyer)\n");
		exit(1);
	}
	for(i=0;i<FILHOS;i++) {
		sprintf(name,"sinc_%d",i);
		if(sem_unlink(name)==-1) {
			perror("Erro no sem_unlink(sem_sinc)\n");
			exit(1);
		}
	}
}
