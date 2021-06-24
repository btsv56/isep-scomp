/*
 * ex01.c
 *
 *  Created on: 13/03/2020
 *      Author: isep
 *  Revised By:
 * 		- Pedro Brandao - 1180715
 * 		- João Leal - 1180723
 * 		- Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
	int fd[2];
	pid_t p;
	if(pipe(fd) == -1) { /* Verifica se o piper foi criado corretamente */
		perror("Erro a criar o pipe\n");
		exit(1);
	}
	p=fork();
	if(p==-1) { /* Verifica se o fork foi efetuado corretamente */
		perror("Erro a criar o fork");
		exit(1);
	}
	if(p>0) {
		close(fd[0]); 	/* fecha a extremidade de leitura */
		int pid= getpid();
		printf("PID a partir do pai: %d\n",pid);
		write(fd[1],&pid,sizeof(pid)); 	/* Escreve para o pipe o pid do processo pai */
		close(fd[1]); 	/* fecha  a extremidade de escrita */
	}else {
		close(fd[1]); 	/* fecha  a extremidade de escrita */
		int pid;
		read(fd[0],&pid,sizeof(pid)); /* Lê do pipe o pid do processo pai */
		printf("PID a partir do filho: %d\n",pid);
		close(fd[0]); 	/* fecha a extremidade de leitura */
		exit(0);
	}
	exit(0);
}

