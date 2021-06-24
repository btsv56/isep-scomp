/*
 * ex05.c
 *
 *  Created on: 13/03/2020
 *      Author: Bruno Veiga
 * 
 *  Revised by:
 * 		João Leal - 1180723
 * 		Bruno Ribeiro - 1180573
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define STR_LEN 256
#define LEITURA 0
#define ESCRITA 1

int main() {
	int fd_up[2], fd_down[2], i, status;
	char mensagem[STR_LEN];
	pid_t p;
	if (pipe(fd_up) == -1) { /* Verifica se o pipe foi bem criado */
		perror("Não é possivel criar o pipe up\n");
		exit(1);
	}
	if (pipe(fd_down) == -1) {
		perror("Não é possível criar o pipe down\n");
		exit(1);
	}
	p = fork();
	if (p == -1) { /* Verifica se o fork foi bem executado */
		perror("Não é possivel fazer fork\n");
		exit(1);
	}
	if (p > 0) {
		close(fd_up[ESCRITA]);  /* Fecha a extremidade de escrita do pipe up */
		close(fd_down[LEITURA]); /* Fecha a extremidade de leitura do pipe down */
		read(fd_up[LEITURA], mensagem, STR_LEN); /* Le a mensagem do pipe */
		for (i = 0; i < strlen(mensagem) + 1; i++) {
			if (mensagem[i] >= 'a' && mensagem[i] <= 'z') { /* Transforma os caracteres minusculos em maiusculos */
				mensagem[i] -= 32;
			} else if (mensagem[i] >= 'A' && mensagem[i] <= 'Z') { /* Transforma os caracteres maiusculos em minusculos */
				mensagem[i] += 32;
			}
		}
		write(fd_down[ESCRITA], mensagem, strlen(mensagem) + 1); /* Envia a mensagem alterada para o filho */
		close(fd_up[LEITURA]); /* Fecha a extremidade de leitura do pipe up */
		close(fd_down[ESCRITA]); /* Fecha a extremidade de escrita do pipe down */
 		wait(&status);
	} else {
		close(fd_up[LEITURA]);
		close(fd_down[ESCRITA]);
		printf("Escreva a mensagem\n");
		fgets(mensagem, STR_LEN, stdin);  /* Recebe a mensagem do utilizador */
		write(fd_up[ESCRITA], mensagem, strlen(mensagem) + 1); /* Envia a mensagem que utilizador escreveu para o pai */
		read(fd_down[LEITURA], mensagem, STR_LEN); /* Lê a mensagem alterada que o pai enviou */
		printf("Mensagem: %s\n", mensagem);
		close(fd_up[ESCRITA]);
		close(fd_down[LEITURA]);
		exit(0);
	}
	exit(0);
}
