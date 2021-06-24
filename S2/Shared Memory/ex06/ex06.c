/*
 * ex06.c
 *
 *  Created on: 27/03/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 * 	Revised by: Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define SIZE 1000000

typedef struct {
	int numero;
	int write;
	int read;
} array_numeros_type;

void encher_array(int *array) {
	int i;
	srand((unsigned) time(NULL));
	for (i = 0; i < SIZE; i++) {
		array[i] = rand() % 100;
	}
}

int main() {
	clock_t start, end;
	pid_t p;
	int status, fd, data_size = sizeof(array_numeros_type),l1,l2,numeros[SIZE],numeros_duplicados[SIZE];
	encher_array(numeros);  /* Atribuição de numeros ao array */
	array_numeros_type *array_numeros;

	/* Verificação de velocidade de memoria partilhada */
	fd = shm_open("/shmex06", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); /* Abertura da zona de memoria partilhada*/
	if (fd == -1) {
		perror("Falha na criação de memoria partilhada\n");
		exit(1);
	}
	if (ftruncate(fd, data_size) == -1) {
		perror("Não foi possivel alocar espaço para a memoria partilhada\n");
		exit(1);
	}
	array_numeros = (array_numeros_type*) mmap(NULL, data_size,
	PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (array_numeros == MAP_FAILED) {
		perror("Erro no mmap");
		exit(1);
	}
	/* Atribuição de numeros ao array*/
	array_numeros->write = 1;
	array_numeros->read = 0;
	p = fork();
	if (p == -1) {
		perror("Ocorreu um erro no fork\n");
		exit(1);
	}
	if (p > 0)  {
		start = clock();
		for(l1=0;l1<SIZE;l1++) {
			while(array_numeros->write!=1);
			array_numeros->numero = numeros[l1]; /* Colocação de um elemento na zona de memoria partilhada */
			array_numeros->write = 0;
			array_numeros->read=1;
		}
	} else {
		for(l2=0;l2<SIZE;l2++) {
			while (array_numeros->read != 1); /* Mecanismo de espera ativa */
			numeros_duplicados[l2] = array_numeros->numero; /* Obtenção de cada valor do array através da memoria partilhada*/
			/*printf("Exemplo de numeros do array: %d, %d, %d \n",numeros_duplicados[0],numeros_duplicados[10],numeros_duplicados[100]);*/
			array_numeros->read=0;
			array_numeros->write=1;
		}
		if (munmap(array_numeros, data_size) < 0) {  /* Fecho da zona de memoria partilhada */
			perror("Erro no munmap\n");
			exit(1);
		}
		if(close(fd)==-1) {
			perror("Erro no close(fd)\n");
			exit(1);
		}
		exit(0);
	}
	wait(&status);
	end = clock();
	printf("Tempo memoria partilhada: %.6f\n",
			(float) (end - start) / CLOCKS_PER_SEC);

	/* Fecho da zona de memoria partilhada */
	
	if (munmap(array_numeros, data_size) < 0) {  
		perror("Erro no munmap\n");
		exit(1);
	}
	if(close(fd)==-1) {
		perror("Erro no close(fd)\n");
		exit(1);
	}
	if (shm_unlink("/shmex06") == -1) {
		perror("Erro no shm_unlink\n");
		exit(1);
	}

	/* Veriicação de velocidade de pipes */
	int fd_pipe[2];
	if (pipe(fd_pipe) == -1) {
		perror("Erro no pipe\n");
		exit(1);
	}
	p = fork();
	if (p == -1) {
		perror("Erro no fork\n");
		exit(1);
	}
	if (p > 0) {
		start = clock();
		if(close(fd_pipe[0])==-1){perror("Erro no close 0\n");exit(1);}
		for(l1=0;l1<SIZE;l1++) {
			if(write(fd_pipe[1], &numeros[l1], 4)==-1) {perror("Erro no write\n");exit(1);}
		}
		if(close(fd_pipe[1])==-1) {perror("Erro no close 1\n"); exit(1); }
	} else {
		if(close(fd_pipe[1])==-1) {perror("Erro no close 1");exit(1);}
		for(l2=0;l2<SIZE;l2++) {
			if(read(fd_pipe[0], &numeros_duplicados[l2], 4)==-1) {perror("Erro no read\n");exit(1);}
			/*printf("Exemplo de numeros do array: %d, %d, %d \n", numeros_duplicados[0],numeros_duplicados[10],numeros_duplicados[100]);*/
		}
		if(close(fd_pipe[0])==-1) {perror("Erro no close 0\n");exit(1);}
		exit(0);
	}
	wait(&status);
	end = clock();
	printf("Tempo pipes: %.6f\n", (float) (end - start) / CLOCKS_PER_SEC);
	exit(0);
}
