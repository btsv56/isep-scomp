/*
 * ex14.c
 *
 *  Created on: 10/04/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 *  Revised by: Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	int numeros[10];
	int can_read;
	int can_write;
} shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type), numero = 0, cont = 0,index=0;
	shared_data_type *shared_data;
	pid_t p;
	/* Criação da zona de memória partilhada */
	fd = shm_open("/shmex14", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("Falha na criação de memoria partilhada\n");
		exit(1);
	}
	if (ftruncate(fd, data_size) == -1) {
		perror("Falha na função ftruncate\n");
		exit(1);
	}
	shared_data = (shared_data_type*) mmap(NULL, data_size,
	PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("Erro no mmap");
		exit(1);
	}
	/* Inicialização dos valores do index, e de variaveis de sincronização de acesso à zona de memoria partilhada*/
	shared_data->can_read = 0;
	shared_data->can_write = 1;
	/* Criação de  um processo filho */
	p = fork();
	if (p == -1) {
		perror("Erro no fork\n");
		exit(1);
	}
	if (p > 0) {
		while (cont < 30) {
			while (!shared_data->can_write); /* Mecanismo de espera ativa para aceder a zona de memoria partilhada */
			/* Definição da variavel de sicronização de escrita*/
			shared_data->numeros[index] = numero; /* Atribuição de um valor a um dos numeros do array da zona de memoria partilhada a partir do index correto */
			numero += 2; /*Incrementação dos valores que serão enviados ao processo filho */
			cont++;
			index++;
			if((cont)%10==0) {
				index=0;
				shared_data->can_write = 0;
				shared_data->can_read = 1; 
			}
		}
	} else {
		while (cont < 30) {
			while (!shared_data->can_read);
			cont++;
			printf("Numero %d: %d\n", cont,shared_data->numeros[index]); /* Apresentação dos numeros pelo consumidor */
			index++;
			if((cont)%10==0) {		
				index=0;
				shared_data->can_read = 0;
				shared_data->can_write = 1; /* Definição da variavel de sincronização que permite a escrita do pai */
			}
		}
		if (munmap(shared_data, data_size) < 0) { /* Fecho da zona de memoria partilhada */
			perror("Erro no munmap\n");
			exit(1);
		}
		if(close(fd)==-1) {
			perror("Erro no close(fd)\n");
			exit(1);
		}
		exit(0);
	}
	if (munmap(shared_data, data_size) < 0) { /* Fecho da zona de memoria partilhada */
		perror("Erro no munmap\n");
		exit(1);
	}
	if(close(fd)==-1) {
		perror("Erro no close(fd)\n");
		exit(1);
	}
	if (shm_unlink("/shmex14") == -1) {
		perror("Erro no shm_unlink\n");
		exit(1);
	}
	exit(0);
}

