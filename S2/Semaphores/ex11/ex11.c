/*
 * ex11.c
 *
 *  Created on: 24/04/2020
 *      Author: isep
 *  Revised by: Bruno Veiga - 1180712
 *  Revised by: João Leal - 1180723
 *  Revised by: Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define FILHOS 205
#define MAXLOT 200

typedef struct {
	int passageiros;
}train;

int criar_filhos() {
	int i;
	pid_t p;
	for(i=1;i<=FILHOS;i++) {
		  p = fork();
		  if (p == 0){
			  return(i);
		  } else if (p == -1){
			  return -1;
		  }
	}
	return 0;
}

int main() {
	sem_t *sem_portas /* Controlo de movimento nas portas */ ,*sem_lot /* Controlo de lotaçao */, *sem_pass /* Controlo da valiavel pasageiros da estrutura train */;
	int fd,data_size=sizeof(train),id,i,status;
	train *comboio;
	/* Abrir zona de memoria partilhada */
	fd=shm_open("/smex11",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	if(fd==-1) {
		perror("Erro no shm_open()\n");
		exit(1);
	}
	if(ftruncate(fd,data_size)==-1) {
		perror("Erro no ftruncate()\n");
		exit(1);
	}
	comboio=(train*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(comboio==MAP_FAILED) {
		perror("Erro no mmao()\n");
		exit(1);
	}
	/*Criar semaforos */
	if((sem_portas=sem_open("semportas",O_CREAT|O_EXCL, 0644,3))==SEM_FAILED) {
		perror("Erro no sem_open() portas\n");
		exit(1);
	}
	if((sem_lot=sem_open("semlotacao",O_CREAT|O_EXCL, 0644,200))==SEM_FAILED) {
		perror("Erro no sem_open() lotacao\n");
		exit(1);
	}
	if((sem_pass=sem_open("sempassageiros",O_CREAT|O_EXCL, 0644,1))==SEM_FAILED) {
		perror("Erro no sem_open() passageiros\n");
		exit(1);
	}
	/* Criação de processos filho */
	id=criar_filhos();
	if(id==-1) {
		perror("Erro na criação dos filhos\n");
		exit(1);
	}
	/* Controlo de lotação e entrada e saida nas portas */
	if(sem_wait(sem_lot)==-1) { perror("Erro no sem_wait(sem_lot)\n");exit(1);} /* Espera que exista um lugar disponivel */
	if(sem_wait(sem_portas)==-1) {perror("Erro no sem_wait(sem_portas)\n");exit(1);} /* Espera que uma porta esteja livre */
	if(sem_wait(sem_pass)==-1) {perror("Erro no sem_wait(sem_pass)\n");exit(1);} /* Controlo da variavel de numero de passageiros */
	comboio->passageiros++;
	printf("Entrou um passageiro: %d Numero de passageiros total: %d\n",id,comboio->passageiros);
	if(sem_post(sem_pass)==-1) {perror("Erro no sem_post(sem_pass)\n");exit(1);}
	if(sem_post(sem_portas)==-1) {perror("Erro no sem_post(sem_portas)\n");exit(1);} /* Depois de o passageiro entrar a porta fica livre outra vez*/
	sleep(5);
	if(sem_wait(sem_portas)==-1) {perror("Erro no sem_wait(sem_portas)\n");exit(1);} /* O passageiro tenta sair por uma das portas disponiveis */
	if(sem_wait(sem_pass)==-1) {perror("Erro no sem_wait(sem_pass)\n");exit(1);} /* Alteração do numero de passageiros que saiu */
	comboio->passageiros--;
	printf("Fim da viagem: %d Numero de passageiros total: %d\n",id,comboio->passageiros);
	if(sem_post(sem_pass)==-1) {perror("Erro no sem_post(sem_pass)\n");exit(1);}
	if(sem_post(sem_portas)==-1){perror("Erro no sem_post(sem_portas)\n");exit(1);} /* Passageiro liberta a porta depois de sair */
	if(sem_post(sem_lot)==-1) {perror("Erro no sem_post(sem_lost)\n");exit(1);} /* Permite que novos passageiros entrem */
	if(id>0) {
		if (munmap(comboio, data_size) == -1) {
			perror("Erro no munmap()\n");
			exit(1);
		}
		if(close(fd)==-1) {
			perror("Erro no close(fd)\n");
			exit(1);
		}
		exit(0);
	}else{
		for(i=0;i<FILHOS;i++) {
			wait(&status);
		}
	}
	/*Remove a zona de memoria partilhada */
	if (munmap(comboio, data_size) == -1) {
		perror("Erro no munmap()\n");
		exit(1);
	}
	if(close(fd)==-1) {
		perror("Erro no close(fd)\n");
		exit(1);
	}
	if (shm_unlink("/smex11") == -1) {
		perror("erro no shm_unlink()\n");
		exit(1);
	}
	/* Elimna os semaforos */
	if(sem_unlink("semportas")==-1) {
		perror("Erro no sem_unlink(semportas)\n");
		exit(1);
	}
	if(sem_unlink("semlotacao")==-1) {
		perror("Erro no sem_unlink(semlotacao)\n");
		exit(1);
	}
	if(sem_unlink("sempassageiros")==-1) {
		perror("Erro no sem_unlink(sempassageiros\n");
		exit(1);
	}
	exit(0);
}
