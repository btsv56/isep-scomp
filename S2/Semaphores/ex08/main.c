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
#define SHM_NAME "shm"
#define MAX 50

int main()
{
    /*Declarações de Variáveis*/
    int fd;
    sem_t *s1;
    struct SHM 
    {
        int n_S;        //Número de S's
        int n_C;        //Número de C's
        int i;          //Número de letras
    } * share_data;

    /*Criação dos Semáforos*/
    s1 = sem_open_(S1_NAME, O_CREAT | O_EXCL, 777, 1);


    /*Criação de uma zona de Memória Partilhada*/
    fd = shm_open_(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IWUSR | S_IRUSR);
    ftruncate_(fd, sizeof(struct SHM));
    share_data = mmap_(NULL, sizeof(struct SHM), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*Inicialização dos dados partilhados*/
    share_data->n_C = 0;
    share_data->n_S = 0;        
    share_data->i = 0;

    /*Primeiro Processo*/
    if (fork_() == 0)
    {
        while (share_data->i < MAX)
        {
            sem_wait_(s1);
            if (share_data->n_S - share_data->n_C < 1)  //Se a diferença entre S e C for igual a 1, não adiciona S 
            {
                print("S");
                share_data->i++;
                share_data->n_S++;
            }
            sem_post_(s1);
        }

        munmap_(share_data, sizeof(int));
        close_(fd);
        exit(0);
    }

    /*Segundo Processo*/
    if (fork_() == 0)
    {
        while (share_data->i < MAX)
        {
            sem_wait_(s1);
            if (share_data->n_C - share_data->n_S < 1)  //Se a diferença entre C e S for igual a 1, não adiciona S 
            {
                print("c");
                share_data->i++;
                share_data->n_C++;
            }
            sem_post_(s1);
        }

        munmap_(share_data, sizeof(int));
        close_(fd);
        exit(0);
    }

    /*Processo Pai espera pelos Filhos terminarem*/
    wait_(NULL);
    wait_(NULL);
    
    printf("\n");

    /*Libertação do Semáforo*/
    sem_unlink_(S1_NAME);


    /*Libertação da zona de Memória Partilhada*/
    munmap_(share_data, sizeof(int));
    close_(fd);
    shm_unlink_(SHM_NAME);

    return 0;
}