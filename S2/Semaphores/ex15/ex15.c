/*
* Revised by: Bruno Veiga - 1180712
* Revised by: Bruno Ribeiro - 1180573
*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/wait.h>
#include "../share/functions.h"

#define SHM_NAME "/ex15"
#define N_SEMS 2
#define N_PEOPLE 10
#define SEM1 "entrada"
#define SEM2 "mutex"
#define OPEN_TIME 2

/*
 * this structure will be used to communicate
 * between processes the ammount of people that
 * are currently inside the room
 */
typedef struct{
    int n_visitors;
} show_room;

int main(){

    time_t start_t, now_t;
    sem_t *entrada;
    sem_t *mutex;
    const char sem_name[N_SEMS][20] = {SEM1, SEM2};
    int i, id, fd;
    show_room *room;

    /*Creates a shared memory space*/
    fd = shm_open_(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, sizeof(room));
    room = mmap_(NULL, sizeof(room), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*initializes the values in the shared memory space*/
    room->n_visitors = 0;

    /*
     * Controla a entrada na sala. O semaforo é iniciado com o valor 5 para permitir 5 pessoas
     * dentro da sala
     */
    if((entrada = sem_open(sem_name[0], O_CREAT | O_EXCL, 0644, 5)) == SEM_FAILED){
        perror("Error creating semaphore\n");
        exit(1);
    }

    /*
     * Controla a alteração feita no espaço de memória partilhado
     */
    if((mutex = sem_open(sem_name[1], O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        perror("Error creating semaphore\n");
        exit(1);
    }

    start_t = time(NULL);

    id = spawn_childs(N_PEOPLE);

    /* Apenas os processos filhos executam este código*/
    if(id > 0) {

        now_t = time(NULL);
        /*Verifica se já está na hora de o show room começar, através de um mecanismo de espera ativa
         * que a cada 1 segundo verifica se já chegou o tempo
         */
        while ((now_t - start_t) < OPEN_TIME) {
            printf("Cant enter yet\n");
            sleep(1);
            now_t = time(NULL);
        }

        sem_wait_(entrada);
        sem_wait_(mutex);
        room->n_visitors++;
		/*
         * Entra no show room
         */
        printf("I Got in! pid %d Visitors: %d\n", getpid(),room->n_visitors);
        sem_post_(mutex);

        
        sleep(3);
        sem_post_(entrada);
		
        sem_wait_(mutex);
        room->n_visitors--;
        /* sai do show room */
        printf("I got out pid %d Visitors: %d\n", getpid(), room->n_visitors);
        sem_post_(mutex);

        exit(EXIT_SUCCESS);
    }

    /*
     * fecha a shm
     */
    close_(fd);

    /*
     * espera pelos filhos
     */
    for(i = 0; i < N_PEOPLE; i++){
        wait_(NULL);
    }

    /*
     * fehca os semaforos
     */
    sem_close_(entrada);
    sem_close_(mutex);

    /*
     * apaga os semaforos
     */
    sem_unlink_(SEM1);
    sem_unlink_(SEM2);

    /*
     * apaga a shm
     */
    munmap_(room, sizeof(room));
    shm_unlink_(SHM_NAME);

    return 0;
}
