/*
 * ex02.c
 *
 *  Created on: 27/03/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 * 				Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	int code;
	char description[20];
	int price;
} shared_data_type;

int main() {
	int fd, data_size = sizeof(shared_data_type);
	shared_data_type *shared_data;
	fd = shm_open("/shmex2", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
	printf("Escreva o código do produto\n");
	scanf("%d", &(shared_data->code));
	while ((getchar()) != '\n')
		;
	printf("Escreva a descricao do produto\n");
	fgets(shared_data->description, 20, stdin);
	printf("Insira o preço\n");
	scanf("%d", &(shared_data->price));
	if (munmap(shared_data, data_size) < 0) { /* Fecho da zona de memoria partilhada */
		perror("Erro no munmap\n");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("Error no close\n");
		exit(1);
	}
	exit(0);
}
