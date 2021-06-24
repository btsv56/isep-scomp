/*
 * Author: Bruno Ribeiro - 1180573
 * Revised by:
 *      - Bruno Veiga - 1180712
 *      - João Leal - 1180723
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define N_CHILDS 5
#define N_PROCESS N_CHILDS + 1

/* Cria o numero de filhos pretendidos, cada um retornando o valor da ordem de sua criacao
 * param n: Numero de processos filhos a serem criados
 * return: retorna o numero atribuido ao processo criado*/
int forking(int n) {
    pid_t p; /* pid of child process */
    int i;

    for (i = 1; i <= n; i++) {

        p = fork();
        if (p == 0) {
            return i;
        } else if (p == -1) {
            return -1;
        }

    }
    return 0;
}

/* Cria o numero de pipes necessarios
 * param *v: Array de descritores
 * param n: numero de pipes a serem criados
 * return: retorna 1 se houver algum erro ao criar o pipe; retorna 0 se correr bem*/
int pipes(int *v, int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (pipe(v + i * 2) == -1) {
            return 1;
        }
    }

    return 0;
}

/* calcula o index de escrita do processo no contexto do exercicio
 * param *w: pontador para o index de escrita
 * param p: numero do processo*/
void getWriteIndex(int *w, int p) {
    *w = (p << 1) + 1;
}

/* calcula o index de leitura do processo no contexto do exercicio
 * param *r: pontador para o index de leitura
 * param p: numero do processo*/
void getReadIndex(int *r, int p) {
    if (p != 0) {
        *r = (p << 1) - 2;
    } else {
        *r = N_CHILDS << 1;
    }
}

/* fecha todos os pipes que nao forem usados pelo processo */
void closePipes(int p, int *fd) {
    int w, r;
    int i;

    getReadIndex(&r, p);
    getWriteIndex(&w, p);

    for (i = 0; i < ((N_PROCESS) << 1); i++) { /*j <= N_CHILDS <==> j < N_CHILDS + 1*/
        if (w != i && r != i) {
            close(fd[i]);
        }
    }
}

/* Sai do programa com erro */
void brexit() {
    perror("A wild error appeared... Exiting application.");
    exit(1);
}


int main() {
    int p; /* numero atribuido a um processo */
    int fd[(N_PROCESS) << 1]; /* array de descritores */
    int n; /* numero aleatorio */
    int w, r; /* index de escrita e de leitura, respetivamente */
    int temp1, temp2; /* variaveis de auxilio para calculos temporarios */


    /* cria os pipes todos */
    if (pipes(fd, N_PROCESS)) {
        brexit();
    }

    /* cria os processos filhos todos */
    p = forking(N_CHILDS);
    if (p == -1) {
        brexit();
    }

    /* para cada processo fecha os pipes nao usados */
    closePipes(p, fd);

    /* obtem os indices dos descritores de escrita e leitura para cada processo */
    getWriteIndex(&w, p);
    getReadIndex(&r, p);

    /* gera um numero aleatorio de 1 a 500*/
    srand(time(NULL) * p);
    n = (rand() % 500) + 1;

    /* escreve o pid do respetivo processe e o nuemro aleatorio gerado */
    printf("PID: %d (p:%d)\nNumber: %d\n\n", getpid(), p, n);

    /* se for o pai ele manda para o filho seguinto o respetivo numero gerado e depois le o valor devolvido pelo ultimo filho */
    if (p == 0) {
        write(fd[w], &n, sizeof(int));
        close(fd[w]);
        read(fd[r], &n, sizeof(int));
        close(fd[r]);
        printf("\n\nn : %d\n\n", n);
        printf("Greatest Number: %d\n\n", n);

    /* cada filho ira pegar no valor lido pelo pipe e vai verificar qual deles e o maior escrevendo no pipe seguinte o maior dos dois */
    } else {
        read(fd[r], &temp1, sizeof(int));
        close(fd[r]);
        if (temp1 > n) {
            temp2 = temp1;
        } else {
            temp2 = n;
        }
        write(fd[w], &temp2, sizeof(int));
        close(fd[w]);

        exit(0);
    }


    return 0;
}
