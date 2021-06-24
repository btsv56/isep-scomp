/**************************************
 * Autor: Bruno Ribeiro - 1180573     *
 *                                    *
 * Revised by: Bruno Veiga - 1180712  *
 * Revised by: João Leal - 1180723    *
 * Revised by: Bruno Ribeiro - 1180573*
 **************************************/

#include <stdio.h>
#include "../share/functions.h"

#define S1_NAME "full"
#define S2_NAME "empty"
#define S3_NAME "mutex"
#define SHM_NAME "shm"
#define MAX 30
#define BUFFER_SIZE 10

int next_(int index)
{
    if (BUFFER_SIZE - 1 == index)
    {
        return 0;
    }

    return index + 1;
}

int main()
{
    /*Declaração de Varáveis*/
    sem_t *full, *empty, *mutex;
    int flag = 1;
    int fd;
    int i = 0, count = 0;
    struct SHM
    {
        int buffer[BUFFER_SIZE];
        int head;
        int tail;
        int n_values;
    } * sh_m;

    /*Inicialização da Memória Partilhada*/
    fd = shm_open_(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, sizeof(struct SHM));
    sh_m = mmap_(NULL, sizeof(struct SHM), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*Inicialização de Semáforos*/
    full = sem_open_(S1_NAME, O_CREAT | O_EXCL, S_IRWXU, 0);
    empty = sem_open_(S2_NAME, O_CREAT | O_EXCL, S_IRWXU, BUFFER_SIZE);
    mutex = sem_open_(S3_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);

    /*Inicialização de Variáveis*/
    sh_m->n_values = 0;
    sh_m->head = 0;
    sh_m->tail = 0;

    /*Producer 01*/
    if (fork_() == 0)
    {
        while (flag)
        {
            i++;

            sem_wait_(empty);
            sem_wait_(mutex);
            if (sh_m->n_values >= MAX)
            {
                sem_post_(mutex);
                sem_post_(empty);
                flag = 0;
            }
            else
            {
                sh_m->buffer[sh_m->head] = i;
                printf("Producer 01 prints %d\n", sh_m->buffer[sh_m->head]);
                sh_m->head = next_(sh_m->head);
                sh_m->n_values++;
                sem_post_(mutex);
                sem_post_(full);
            }
        }
        
        munmap_(sh_m, sizeof(struct SHM));
        close_(fd);
        exit(0);
    }

    /*Producer 02*/
    if (fork_() == 0)
    {
        while (flag)
        {
            i++;

            sem_wait_(empty);
            sem_wait_(mutex);
            if (sh_m->n_values >= MAX)
            {
                sem_post_(mutex);
                sem_post_(empty);
                flag = 0;
            }
            else
            {
                sh_m->buffer[sh_m->head] = i;
                printf("Producer 02 prints %d\n", sh_m->buffer[sh_m->head]);
                sh_m->head = next_(sh_m->head);
                sh_m->n_values++;
                sem_post_(mutex);
                sem_post_(full);
            }
        }

        munmap_(sh_m, sizeof(struct SHM));
        close_(fd);
        exit(0);
    }

    /*Consumer*/
    while (count < MAX)
    {
        sem_wait_(full);
        sem_wait_(mutex);
        i = sh_m->buffer[sh_m->tail];
        printf("Consumer prints %d\n", sh_m->buffer[sh_m->tail]);
        sh_m->tail = next_(sh_m->tail);
        sem_post_(mutex);
        sem_post_(empty);

        count++;
    }


    wait_(NULL);
    wait_(NULL);

    /*Remove os Semáforos*/
    sem_unlink_(S1_NAME);
    sem_unlink_(S2_NAME);
    sem_unlink_(S3_NAME);

    /*Remove a Memória Partilhada*/
    munmap_(sh_m, sizeof(struct SHM));
    close_(fd);
    shm_unlink_(SHM_NAME);

    return 0;
}