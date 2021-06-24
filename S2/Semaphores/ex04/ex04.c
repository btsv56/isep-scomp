/*
 * ex03.c
 *
 *  Created on: 17/04/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 *  Revised by: Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define STR_NUMBER 50
#define STR_SIZE 80
#define N 1
#define CHILD_NUMBER 10
#define SECONDS 12

typedef struct {
	char strings[STR_NUMBER][STR_NUMBER];
	int index;
} shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type), i, status;
	shared_data_type *shared_data;
	pid_t pid;
	sem_t *sem;
	struct timespec time;
    /* Definição do tempo de timeout */
	if (clock_gettime(CLOCK_REALTIME, &time) == -1) {
        perror("Erro no clock_gettime");
        exit(1);
    }
	time.tv_sec+=SECONDS;
	/* Abertura da zona de memoria partilhada */
	fd = shm_open("/shmex04", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("Erro no shm_open()");
		exit(1);
	}

	if (ftruncate(fd, data_size) == -1) {
		perror("Erro no ftruncate()");
		exit(1);
	}

	shared_data = (shared_data_type *) mmap(NULL, data_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
		perror("Erro no mmap");
		exit(1);
	}
	shared_data->index = 0;
	/* Criação do semaforo */
	if ((sem = sem_open("semaphore04", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("Error in sem_open()");
		exit(1);
	}
	/* Criação dos processos filhos que iram escrever as mensagens na zona de memoria partilhada */
	for (i = 0; i < CHILD_NUMBER; i++) {
		pid = fork();

		if (pid == -1) {
			perror("Erro no fork()");
		}
		if (pid == 0) {
			if (sem_timedwait(sem, &time) == -1) {
				perror("Passaram mais de 12 segundos, saindo\n");
				exit(1);
			}
			if (shared_data->index == STR_NUMBER) {
				shared_data->index = 0;
			}
			sprintf(shared_data->strings[shared_data->index],
					"I’m the Father - with PID %d", getpid());
			shared_data->index++;
			sleep(N);
			if(sem_post(sem)==-1) {
				perror("Erro no sem_post(sem)\n");
				exit(1);
			}
			if (munmap(shared_data, data_size) == -1) {
				perror("Erro no munmap()\n");
				exit(1);
			}
			if(close(fd)==-1) {
				perror("Erro no close(fd)");
				exit(1);
			}
			exit(0);
		}
	}
	for (i = 0; i < CHILD_NUMBER; i++) {
		wait(&status);
	}
	printf("Deseja remover alguma string?(s/n)\n");
	char r;
	scanf("%c", &r);
	if (r == 's') {
		printf("Indique o index que pretende remover\n");
		int n;
		scanf("%d", &n);
		if (n < STR_NUMBER) {
			strcpy(shared_data->strings[n], "");
		}
	}
	/* Apresentação do conteudo da memoria partilhada */
	for (i = 0; i < shared_data->index; i++) {
		if (strcmp(shared_data->strings[i],"")!=0) {
			printf("%s\n", shared_data->strings[i]);
		}
	}
	/* Fechar zona de memoria partilhada */
	if (munmap(shared_data, data_size) == -1) {
		perror("Erro no munmap()\n");
		exit(1);
	}
	if(close(fd)==-1) {
		perror("Erro no close(fd)");
		exit(1);
	}
	if (shm_unlink("/shmex04") == -1) {
		perror("erro no shm_unlink()\n");
		exit(1);
	}

	/*Fechar semaforo */
	if(sem_unlink("semaphore04")==-1) {
		perror("Erro no sem_unlink(semaphore3)\n");
	}
	exit(0);
}

