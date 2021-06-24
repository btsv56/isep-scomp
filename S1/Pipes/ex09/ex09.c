/*
 * ex09.c
 *
 *  Created on: 15/03/2020
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

#define TOTAL 50000
#define LEITURA 0
#define ESCRITA 1

typedef struct {
	int customer_code;
	int product_code;
	int quantity;
} Sales;

int main() {
	int fd[2], i,j, status, products[TOTAL],code;
	pid_t p;
	Sales sales[TOTAL];
	for (i = 0; i < TOTAL; i++) { /* atribui valores às vendas */
		sales[i].customer_code = rand() % 20;
		sales[i].product_code = i;
		sales[i].quantity = rand() % 30;
	}
	if (pipe(fd) == -1) { /* Verifica se o pipe foi bem criado */
		perror("Não é possivel criar o pipe\n");
		exit(1);
	}
	for (i = 0; i < 10; i++) {
		p = fork();  /* Criação dos 10 processos filhos */
		if (p == -1) { /* Verifica se o fork foi bem executado */
			perror("Não foi possível criar o fork!\n");
			exit(1);
		}
		if (p == 0) { 	/* Código dos filhos que permite a verificação de 5000 vendas cada um */
			close(fd[LEITURA]); 	/* Fecha a extremidade de leitura do pipe */
			for (j = 5000 * i; j < 5000 * (i + 1); j++) {
				if (sales[j].quantity > 20) { 	/* Verifica se a quantidade da venda é superior a 20 */
					/* envia o codigo do produto ao processo pai caso a quantidade da venda seja superior a 20 */
					write(fd[ESCRITA], &(sales[j].product_code),sizeof(sales[j].product_code));
				}
			}
			close(fd[ESCRITA]); /* Fecha a extremidade de escrita do pipe */
			exit(0);
		}
	}
	close(fd[ESCRITA]); /* Fecha a extremidade de escrita do pipe */
	j=0;
	while(read(fd[LEITURA],&code,sizeof(int))) { /* Enquanto existir codigos de produtos para ler do pipe */
		products[j]=code; /* Coloca os codigos recebidos dos processos filhos no array products */
		j++;
	}
	close(fd[LEITURA]);
	for(i=0;i<10;i++) {
		wait(&status);  /* Espera que todos os processos filhos terminem */
	}
	printf("Products:\n");
	for(i=0;i<j;i++) {
		printf("%d\n",products[i]); /* Apresenta todos os códigos de produtos que foram vendidios com uma quantidade acima de 20 */
	}
	exit(0);

}
