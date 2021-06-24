/**************************************
 * Autor: Bruno Ribeiro - 1180573     *
 *                                    *
 * Revised by: Bruno Veiga - 1180712  *
 **************************************/

#include <stdio.h>
#include "../share/functions.h"
#include <string.h>
#include <time.h>

#define S1_NAME "full"
#define S2_NAME "empty"
#define S3_NAME "mutex"
#define S4_NAME "mutex1"
#define S5_NAME "mutex2"
#define S6_NAME "mutex3"
#define SHM_NAME "shm"
#define READERS 30
#define WRITERS 10
#define PRINTS 5

int main()
{
    /*Declaração de Varáveis*/
    sem_t *sem_read, *sem_write, *mutex_readers, *mutex_writers, *mutex, *barrier;
    pid_t p[READERS + WRITERS];
    time_t rawtime;
    int fd;
    int i;
    char s[100];
    struct SHM
    {
        char string[100];
        int readers;
        int writers;
    } * sh_m;

    /*Inicialização da Memória Partilhada*/
    fd = shm_open_(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, sizeof(struct SHM));
    sh_m = mmap_(NULL, sizeof(struct SHM), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*Inicialização de Semáforos*/
    sem_write = sem_open_(S1_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    sem_read = sem_open_(S2_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    mutex_readers = sem_open_(S3_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    mutex_writers = sem_open_(S4_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    mutex = sem_open_(S5_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    barrier = sem_open_(S6_NAME, O_CREAT | O_EXCL, S_IRWXU, 0);

    strcpy(sh_m->string, "empty");
    sh_m->readers = 0;
    sh_m->writers = 0;

    /*Readers*/
    for (i = 0; i < READERS; i++)
    {
        p[i] = fork_();
        if (p[i] == 0)
        {
            sem_wait_(barrier);
            sem_post_(barrier);

            sem_wait_(mutex);
            sem_wait_(sem_read);
            sem_wait_(mutex_readers);
            sh_m->readers++;
            if (sh_m->readers == 1)
            {
                sem_wait_(sem_write);
            }
            sem_post_(mutex_readers);
            sem_post_(sem_read);
            sem_post_(mutex);

            strcpy(s, sh_m->string);

            sem_wait_(mutex_readers);
            printf("Reader[%d] --> String: \"%s\"; Number of Readers: %d\n", i, s, sh_m->readers);
            sh_m->readers--;
            if (sh_m->readers == 0)
            {
                sem_post_(sem_write);
            }
            sem_post_(mutex_readers);

            munmap_(sh_m, sizeof(struct SHM));
            close_(fd);
            exit(0);
        }
    }

    /*Writers*/
    for (i = READERS; i < READERS + WRITERS; i++)
    {
        p[i] = fork_();
        if (p[i] == 0)
        {   
            sem_wait_(barrier);
            sem_post_(barrier);
            
            sem_wait_(mutex_writers);
            sh_m->writers++;
            if (sh_m->writers == 1)
            {   
                sem_wait_(sem_read);
            }
            sem_post_(mutex_writers);

            sem_wait_(sem_write);
            time(&rawtime);
            sprintf(sh_m->string, "%d >-> %s", getpid(), strtok(asctime(localtime(&rawtime)), "\n"));
            sem_post_(sem_write);


            sem_wait_(mutex_writers);

            sem_wait_(mutex_readers);
            printf("Writer[%d] --> Number of Writers: %d; Number of Readers: %d\n", i, sh_m->writers, sh_m->readers);
            sem_post_(mutex_readers);
            
            sh_m->writers--;
            if (sh_m->writers == 0)
            {
                sem_post_(sem_read);
            }
            sem_post_(mutex_writers);
            
            munmap_(sh_m, sizeof(struct SHM));
            close_(fd);
            exit(0);
        }
    }

    close_(fd);

    sem_post_(barrier);

    for (i = 0; i < READERS + WRITERS; i++)
    {
        wait_(NULL);
    }

    /*Remove os Semáforos*/
    sem_unlink_(S1_NAME);
    sem_unlink_(S2_NAME);
    sem_unlink_(S3_NAME);
    sem_unlink_(S4_NAME);
    sem_unlink_(S5_NAME);
    sem_unlink_(S6_NAME);

    /*Remove a Memória Partilhada*/
    munmap_(sh_m, sizeof(struct SHM));
    shm_unlink_(SHM_NAME);

    return 0;
}