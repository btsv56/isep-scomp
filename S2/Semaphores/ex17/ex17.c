/**
 * Author: João Leal©
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#define THEATRE_SIZE 100
#define CHLD_NUM 200
#define VIP_ID 1
#define SPC_ID 2
#define PLB_ID 3

typedef struct {
    int size;
    int spectators;
    int vips;
    int specials;
    int plebes;
} movie_theatre;

/**
 * Lets someone in, prints their ID
 */
void get_in(movie_theatre *theatre, int id) {
    theatre->spectators++;
    printf("Process %d is getting in. (ID: %d)\n", getpid(), id);
    printf("%d spectators.\n\n", theatre->spectators);
}

/**
 * Lets someone out, prints their ID
 */
void get_out(movie_theatre *theatre, int id) {
    theatre->spectators--;
    printf("Process %d is going out. (ID: %d)\n", getpid(), id);
    printf("%d spectators.\n\n", theatre->spectators);
}

/**
 * "Posts" a semaphore
 */
void post(sem_t *sem) {
    if (sem_post(sem)==-1) { perror("Semaphore 'post' failure."); exit(1); }
}

/**
 * "Waits" a semaphore
 */
void waits(sem_t *sem) {
    if (sem_wait(sem)==-1) { perror("Semaphore 'wait' failure."); exit(1); }
}

/**
 * Semaphore behavior:
 * 
 *          - sems[4] makes sure childs enter one at a time to avoid having childs over the theatre limit 
 *          (since this process checks the is_full flag to either let a child in the theatre or to put it in queue. 
 *          If a child is let in at the same time as another, and the first child sets off the flag, the other still gets in);
 * 
 *          - If the child gets in, it posts sems[4], to start the process for another child, 
 *          sleeps for the entire duration of the feature length movie (5 seconds), is woken up by a bouncer and is told to leave.
 *          If the theatre is full, the child enters the queue: posts sems[4], their respective ID counter in the struct is incremented 
 *          (to let the parent know how many childs of that ID are in queue) and waits for their semaphore to post. 
 *          VIPs have sems[1], Specials have sems[2], and plebes have sems[3].
 * 
 *          - sems[0] starts as 0, only posts when theatre becomes full for the first time. 
 *          When sems[0] becomes 1, the parent begins filtering the queue:
 *          It enters a cycle where it waits for sems[6], checks if the theatre is full and, if it isn't, posts sems[6] and repeats. 
 *          If the theatre isn't full (somebody left), it checks if there are VIPs in queue. If there are, it posts their semaphore. 
 *          In turn, the child VIP gets in and posts sems[6], so that the parent only gets to check if it's full after the child enters. 
 *          This is done to avoid overcrowding, since the parent cycle is sometimes faster than a child getting in. 
 *          The parent does this until there are no more VIPs in queue, when it starts doing the same for Specials and then plebes.
 *          When there's no more childs in queue, it breaks the cycle and waits for every child to leave and go home.
 *          
 *          - Meanwhile, sems[5] makes sure the spectator counter is not overwritten.
 */

int main() {
    int fd, i, data_size=sizeof(movie_theatre), id=0;
    sem_t *sems[7];
    pid_t pid;
    movie_theatre *theatre;

    //Initializes shared memory
    fd = shm_open("/shm_ex17", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    theatre = (movie_theatre *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (theatre==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    //Opens semaphores
    if ((sems[0] = sem_open("sem_1", O_CREAT, 0644, 0)) == SEM_FAILED) { perror("1 Semaphore failure."); exit(1); } //posts when theatre is full
    if ((sems[1] = sem_open("sem_2", O_CREAT, 0644, 0)) == SEM_FAILED) { perror("2 Semaphore failure."); exit(1); } //vips
    if ((sems[2] = sem_open("sem_3", O_CREAT, 0644, 0)) == SEM_FAILED) { perror("3 Semaphore failure."); exit(1); } //specials
    if ((sems[3] = sem_open("sem_4", O_CREAT, 0644, 0)) == SEM_FAILED) { perror("4 Semaphore failure."); exit(1); } //plebes
    if ((sems[4] = sem_open("sem_5", O_CREAT, 0644, 1)) == SEM_FAILED) { perror("5 Semaphore failure."); exit(1); } //controls child entry
    if ((sems[5] = sem_open("sem_6", O_CREAT, 0644, 1)) == SEM_FAILED) { perror("6 Semaphore failure."); exit(1); } //controls spectator variable
    if ((sems[6] = sem_open("sem_75", O_CREAT, 0644, 1)) == SEM_FAILED) { perror("7 Semaphore failure."); exit(1);} //controls parent cycle

    //Initializes shared struct
    theatre->size=THEATRE_SIZE;
    theatre->spectators=0;
    theatre->vips=0;
    theatre->specials=0;
    theatre->plebes=0;

    //Creates childs
    for (i=0;i<CHLD_NUM;i++) {
        if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
        if (pid==0) {
            if (i%6==0) id=VIP_ID;                              //Makes it so ~1/6 of all childs are VIPs,
            else if (i%6==1 || i%6==2) id=SPC_ID;               //~1/3 of all childs are Special
            else id=PLB_ID;                                     //and ~1/2 of all childs are plebes
            break; 
        }
    }

    //Movie starts
    if (pid==0) {
        waits(sems[4]);                         //Makes sure childs are getting in one at a time

        if (theatre->spectators<THEATRE_SIZE) {

            waits(sems[5]);
            get_in(theatre, id);                //Child gets in theatre
            post(sems[5]);

            if (theatre->spectators==THEATRE_SIZE) { //if theatre is full activates struct flag and posts full semaphore
                post(sems[0]); 
            }                                   

            post(sems[4]);

            sleep(5);                           //Watches movie

            waits(sems[5]);
            get_out(theatre, id);               //Gets out
            post(sems[5]);

        } else {
            post(sems[4]);

            switch (id) {
                case VIP_ID:
                    theatre->vips++;            //Adds VIP to queue
                    waits(sems[1]);             //Waits for VIP sem

                    waits(sems[5]);
                    get_in(theatre, id);        //Child gets in theatre
                    post(sems[5]);
                    post(sems[6]);

                    theatre->vips--;            //Removes VIP from queue
                    sleep(5);                   //Watches movie

                    waits(sems[5]);
                    get_out(theatre, id);       //Gets out
                    post(sems[5]);

                    break;

                case SPC_ID:
                    theatre->specials++;        //Adds Special to queue
                    waits(sems[2]);

                    waits(sems[5]);             //Waits for Special sem
                    get_in(theatre, id);        //Child gets in theatre
                    post(sems[5]);
                    post(sems[6]);

                    theatre->specials--;        //Removes Special from queue
                    sleep(5);

                    waits(sems[5]);
                    get_out(theatre, id);       //Gets out
                    post(sems[5]);

                    break;

                case PLB_ID:
                    theatre->plebes++;          //Adds plebe to queue
                    waits(sems[3]);             //Waits for plebe sem

                    waits(sems[5]);
                    get_in(theatre, id);        //Child gets in theatre
                    post(sems[5]);
                    post(sems[6]);

                    theatre->plebes--;          //Removes plebe from queue
                    sleep(5);

                    waits(sems[5]);
                    get_out(theatre, id);       //Gets out
                    post(sems[5]);

                    break;

                default:
                    break;
            }
        }
        if (munmap(theatre, data_size) < 0) { perror("Munmap error"); exit(1); }
    
        if (close(fd)==-1) { perror("Closing failure."); exit(1); }

        exit(0); //Child process ends
    }

    if (THEATRE_SIZE<CHLD_NUM) {
        waits(sems[0]);                                     //Waits for theatre to be full
        printf("\n\nTHEATRE IS FULL\n\n");
        while (1) {                                         //--------Cycle begins--------
            waits(sems[6]);                                 //Waits for child to get in before checking
            if (theatre->spectators<THEATRE_SIZE) {         //Checks if theatre is full
                if (theatre->vips>0) {                      //If there are VIPs in queue, lets them in first
                    post(sems[1]);                          //Posts VIP sem
                } else if (theatre->specials>0) {           //If there are no VIPs, lets Specials in
                    post(sems[2]);                          //Posts Special sem
                } else if (theatre->plebes>0) {             //If there are no Specials, lets plebes in
                    post(sems[3]);                          //Posts plebe sem
                } else break;                               //--------Cycle ends--------
            } else post(sems[6]);                           //If theatre is full, checks again and again until it isn't
        }
    }

    for (i = 0; i < CHLD_NUM; i++) { wait(NULL); } //Waits for childs to end watching the movie

    if (munmap(theatre, data_size) < 0) { perror("Munmap error"); exit(1); }
    
    if (close(fd)==-1) { perror("Closing failure."); exit(1); }

    if (shm_unlink("/shm_ex17") < 0) { perror("Unlink error"); exit(1); }

    if (sem_close(sems[0])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_1")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[1])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_2")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[2])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_3")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[3])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_4")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[4])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_5")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[5])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_6")==-1) { perror("sem_close failure."); exit(1); }
    if (sem_close(sems[6])==-1) { perror("sem_close failure."); exit(1); } if (sem_unlink("sem_75")==-1) { perror("sem_close failure."); exit(1); }

    return 0;
}