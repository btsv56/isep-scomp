/*
 * Author: Bruno Ribeiro - 1180573
 * Revised by:
 * 		- Bruno Veiga - 1180712
 *      - João Leal - 1180723
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ELEM 1000
#define N_CHILDS 5
#define MAX_ELEM_CHILD MAX_ELEM/N_CHILDS

/* escreve uma porcao do array array */
void printa(int *v){
    int i;

    for (i = 0; i < 10; i++) {
        printf("%d - ", v[i]);
    }

    printf("\n\n");
}

int main() {
    /* os dois vetores com os 1000 elementos */
    int vec1[MAX_ELEM];
    int vec2[MAX_ELEM];

    int fd[N_CHILDS * 2]; /* o array com os descritores para cada pipe dos filhos */

    int value[2]; /* o primeiro valor vai conter o index do array e o segundo a soma dos dois vetores (vec1, vec2) nesse index */
    int temp1, temp2; /* armazenam calculos temporarios */
    int i, j; /* indices */
    int result[MAX_ELEM]; /* array dos resultados das somas */
    pid_t p; /* pid */


    /* Preenchimento dos arrays com valores aleatorios até 10 */
    srand(time(NULL));
    for (i = 0; i < MAX_ELEM; i++) {
        vec1[i] = rand() % 11;
        vec2[i] = rand() % 11;
    }

    /* criacao dos filhos */
    for (i = 0; i < N_CHILDS; i++) {

        /* cria o pipa antes do fork para o respetivo filho, se der erro sai do programa */
        if (pipe(fd + i * 2) == -1) {
            perror("pipe failed");
            exit(1);
        }

        /* cria o processo filho, termina o programa se nao conseguir criar. */
        p = fork();
        if (p == -1) {
            perror("Fork failed");
            exit(1);
        }

        /* se for filho vai pegar numa porcao dos arrays e calcular os resultados enviando para o pipe */
        if (p == 0) {
            close(fd[i * 2]);

            temp1 = i * MAX_ELEM_CHILD;
            for (j = 0; j < MAX_ELEM_CHILD; j++) {
                temp2 = temp1 + j;
                value[0] = temp2;
                value[1] = vec1[temp2] + vec2[temp2];
                write(fd[1 + i * 2], value, sizeof(int) * 2);
            }

            close(fd[1 + i * 2]);
            exit(0);
        }
    }

    /* O processo pai fecha todos os descritores de escrita */
    for (i = 0; i < N_CHILDS; i++) {
        close(fd[1 + i * 2]);
    }

    /* Le todos os valores armazenados nos buffers dos 5 pipes e aramazena no array dos resultados */
    for (i = 0; i < N_CHILDS; i++) {
        do {
            temp1 = read(fd[0 + i * 2], value, sizeof(int) * 2);
            result[value[0]] = value[1];
        } while (temp1 != 0);
    }

    /* Fecha todos os descritores de leitura */
    for (i = 0; i < N_CHILDS; i++) {
        close(fd[0 + i * 2]);
    }

    /* Escrever no ecra uma breve amostra dos resultados obtidos */
    printa(vec1);
    printa(vec2);
    printa(result);

    return 0;
}
