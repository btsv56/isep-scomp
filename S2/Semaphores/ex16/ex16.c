/*
* Revised by: Bruno Veiga - 1180712
* Revised by: Bruno Ribeiro - 1180573
*/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "../share/functions.h"

#define N_SEMS 4
#define N_CHLDS 30
#define WEST "west_side"
#define EAST "east_side"
#define TIME_OF_CROSSING 4

typedef struct {
    int east_queue;
    int west_queue;
} Bridge;

int main() {

    sem_t *west_queue;
    sem_t *east_queue;
    sem_t *east_side;
    sem_t *west_side;

    int fd, data_size=sizeof(Bridge);
    Bridge *bridge;
    int i, pid;

    /*Initializes shared memory*/
    fd = shm_open("/shm_ex16", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    bridge = (Bridge *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bridge==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    /* inicializa as variaveis*/
    bridge->east_queue=0;
    bridge->west_queue=0;

    /* abre os semaforos */
    /* Controlam a permissão de passagem */
    if ((east_side = sem_open(EAST, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) { perror("1 Semaphore failure."); exit(1); }
    if ((west_side = sem_open(WEST, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) { perror("2 Semaphore failure."); exit(1); }
    /* Controlam os acessos */
    if ((east_queue = sem_open("east_queue11", O_CREAT, 0644, 1)) == SEM_FAILED) { perror("3 Semaphore failure."); exit(1); }
    if ((west_queue = sem_open("west_queue11", O_CREAT, 0644, 1)) == SEM_FAILED) { perror("4 Semaphore failure."); exit(1); }

    pid = spawn_childs(N_CHLDS);

    if (pid != 0) {

        if (pid%2==0) {
            /* Lado Este */
            /* certifica que nao ha acessos concorrentes à shm*/
            sem_wait_(east_queue);
            bridge->east_queue++;
            printf("%d cars in east queue\n", bridge->east_queue);
            sem_post_(east_queue);
            /* espera que nao existam carros do lado Este para poder passar */
            sem_wait_(east_side);
            printf("%d is crossing the bridge from the east\n", getpid());
            sem_post_(east_side);
            /* dorme durante o tempo de passagem do carro */
            sleep(TIME_OF_CROSSING);
            /* certifica que nao ha accessos concorrentes à shm*/
            sem_wait_(east_queue);
            bridge->east_queue--;
            printf("%d cars in east queue\n", bridge->east_queue);
            sem_post_(east_queue);
            printf("%d has crossed from the east\n", getpid());
            /* Se o lado este da ponte nao tiver carros em queue permite a passagem no lado oposto */
            if (bridge->east_queue==0) {
                sem_wait_(east_side);
                sem_post_(west_side);
            }
        } else {
            /* Lado Oeste */
            /* Certifica que não há acessos concorrentes à shm*/
            sem_wait_(west_queue);
            bridge->west_queue++;
            printf("%d cars in west queue\n", bridge->west_queue);
            sem_post_(west_queue);
            /* espera que nao existam carros do lado Este para poder passar */
            sem_wait_(west_side);
            printf("%d is crossing the bridge from the west\n", getpid());
            sem_post_(west_side);
            sleep(TIME_OF_CROSSING);
            /* dorme durante o tempo de passagem do carro */
            sem_wait_(west_queue);
            bridge->west_queue--;
            printf("%d cars in west queue\n", bridge->west_queue);
            sem_post_(west_queue);
            printf("%d has crossed from the west\n", getpid());
            /* Se o lado Oeste da ponte nao tiver carros em queue permite a passagem no lado oposto */
            if (bridge->west_queue==0) {
                sem_wait_(west_side);
                sem_post_(east_side);
            }
        }

        munmap_(bridge,data_size);
        close_(fd);

        exit(EXIT_SUCCESS);
    }

    if (bridge->east_queue>=bridge->west_queue) sem_post_(east_side);
    else sem_post_(west_side);

    for (i = 0; i < N_CHLDS; i++) {
        wait_(NULL);
    }

    munmap_(bridge, data_size);
    close_(fd);
    shm_unlink_("/shm_ex16");

    sem_close_(east_side);
    sem_close_(west_side);
    sem_close_(east_queue);
    sem_close_(west_queue);

    sem_unlink_(EAST);
    sem_unlink_(WEST);
    sem_unlink_("east_queue11");
    sem_unlink_("west_queue11");

    return 0;
}