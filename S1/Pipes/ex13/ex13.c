/*
 * ex13.c
 *
 *  Created on: 18/03/2020
 *      Author: Bruno Veiga
 * 
 *  Revised by:
 * 		João Leal - 1180723
 * 		Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define LEITURA 0
#define ESCRITA 1

int main() {
	int fd1[2], fd2[2], fd3[2], fd4[2];
	pid_t p;
	if (pipe(fd1) == -1 || pipe(fd2) == -1 || pipe(fd3) == -1 || pipe(fd4) == -1) { /*Verifica se o pipe foi bem criado*/
		perror("Não é possivel criar o pipe\n");
		exit(1);
	}

	p = fork();
	if (p == -1) { /* Verifica se o fork foi bem executado */
		perror("Não foi possível criar o fork!\n");
		exit(1);
	}
	if (p == 0) { /* Máquina 1 */
		int pieces = 0;
		close(fd1[LEITURA]);
		while (pieces < 1000) {
			pieces = pieces + 5;
			write(fd1[ESCRITA], &pieces, sizeof(pieces));
			printf("5 peças cortadas e enviadas a maquina M2\n");
		}
		close(fd1[ESCRITA]);
		exit(0);
	}

	p = fork();
	if (p == -1) {
		perror("Não foi possível criar o fork!\n");
		exit(1);
	}
	if (p == 0) { /* Máquina 2 */
		int pieces=0;
		close(fd1[ESCRITA]);
		close(fd2[LEITURA]);
		while (pieces<1000) {
			read(fd1[LEITURA], &pieces, sizeof(pieces));
			write(fd2[ESCRITA], &pieces, sizeof(pieces));
			printf("5 peças dobradas e enviadas a maquina M3\n");
		}
		close(fd1[LEITURA]);
		close(fd2[ESCRITA]);
		exit(0);
	}

	p = fork();
	if (p == -1) {
		perror("Não foi possível criar o fork!\n");
		exit(1);
	}
	if (p == 0) { /* Máquina 3 */
		close(fd2[ESCRITA]);
		close(fd3[LEITURA]);
		int pieces=0;
		while (pieces<1000) {
			read(fd2[LEITURA], &pieces, sizeof(pieces));
			if (pieces % 10 == 0) {
				write(fd3[ESCRITA], &pieces, sizeof(pieces));
				printf("10 peças soldadas e enviadas para M4\n");
			}
		}
		close(fd2[LEITURA]);
		close(fd3[ESCRITA]);
		exit(0);
	}

	p = fork();
	if (p == -1) {
		perror("Não foi possível criar o fork!\n");
		exit(1);
	}
	if (p == 0) { /* Máquina 4 */
		close(fd3[ESCRITA]);
		close(fd4[LEITURA]);
		int pieces=0;
		while (pieces<1000) {
			read(fd3[LEITURA], &pieces, sizeof(pieces));
			if (pieces % 100 == 0) {
				write(fd4[ESCRITA], &pieces, sizeof(pieces));
				printf("100 peças empacotadas e enviadas para A1\n");
			}
		}
		close(fd3[LEITURA]);
		close(fd4[ESCRITA]);
		exit(0);
	}

	/* Inventário */
	close(fd4[ESCRITA]);
	int pieces=0;
	while(pieces<1000) {
		read(fd4[LEITURA],&pieces,sizeof(pieces));
		printf("%d peças adicionadas ao inventario\n",pieces);
	}
	close(fd4[LEITURA]);
	return 0;
}
