/**************************************
 * Autor: Bruno Ribeiro - 1180573     *
 *                                    *
 * Revised by: Bruno Veiga - 1180712  *
 * Revised by: João Leal - 1180723    *
 * Revised by: Bruno Ribeiro - 1180573*
 **************************************/

#include <stdio.h>
#include "../share/functions.h"

#define S1_NAME "semaphore1"
#define S2_NAME "semaphore2"
#define S3_NAME "semaphore3"

int main()
{
    /*Declaração de Variáveis*/
    sem_t *s1, *s2, *s3;
	int i=0;

    /* Criação dos Semáforos */
    s1 = sem_open_(S1_NAME, O_CREAT | O_EXCL, 777, 0);
    s2 = sem_open_(S2_NAME, O_CREAT | O_EXCL, 777, 0);
    s3 = sem_open_(S3_NAME, O_CREAT | O_EXCL, 777, 0);

    /* Primeiro Processo*/
    if (fork_() == 0)
    {
        print("Sistemas ");
        sem_post_(s1);//dá luz verde ao Segundo Processo

        sem_wait_(s3);//espera pelo Terceiro Processo

        print("a ");
        sem_post_(s1);//dá luz verde ao Segundo Processo

        exit(0);
    }

    /*Segundo Processo*/
    if (fork_() == 0)
    {
        sem_wait_(s1);//espera pelo Primeiro Processo

        print("de ");
        sem_post_(s2);//dá luz verda ao Terceiro Processo

        sem_wait_(s1);//espera pelo Primeiro Processo, novamente

        print("melhor ");
        sem_post_(s2);//dá luz verda ao Terceiro Processo

        exit(0);
    }

    /*Terceiro Processo*/

    sem_wait_(s2);//espera pelo Segundo Processo

    print("Computadores - ");
    sem_post_(s3);//dá luz verde ao Primeiro Processo

    sem_wait_(s2);//espera pelo Segundo Processo, novamente

    print("disciplina! \n");

    for(i=0;i<2;i++) {
		wait_(NULL);
	}

    /*Remove os Semáforos criados*/
	sem_unlink_(S1_NAME);
    sem_unlink_(S2_NAME);
    sem_unlink_(S3_NAME);

    return 0;
}
