/**************************************
 * Autores:                           *
 *          - Bruno Ribeiro [1180573] *
 *          - Pedro Lopes [1180715]   *
 *          - Bruno Veiga [1180712]   *
 *          - João Leal   [1180725]   *
 *                                    *
 **************************************/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>  
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

/*PROCESSOS*/

pid_t fork_(){
    pid_t p = fork();
    if (p == -1){ 
        perror("fork failed"); 
        exit(1); 
    }
    return p;
}

pid_t wait_(int *status){
    pid_t p = wait(status);
    if (p == -1){
        perror("Failed exiting child");
        exit(1);
    }
    return p;
}


/*PIPES*/

void pipe_(int *fd){
    if (pipe(fd) == -1) { perror("failed pipe"); exit(1);}
}

void close_read(int *fd){
    close_(fd[0]);
}

void close_write(int *fd){
    close_(fd[1]);
}

size_t write_(int *fd, void *buf, size_t nbyte){
    size_t n = write(fd[1], buf, nbyte);
    if (n == -1){
        perror("failed writing in pipe");
        exit(1);
    }
    return n;
}

size_t read_(int *fd, void *buf, size_t nbyte){
    size_t n = read(fd[0], buf, nbyte);
    if (n == -1){
        perror("failed writing in pipe");
        exit(1);
    }
    return n;
}

/*SHARED MEMORY*/

//oflag : O_CREAT | O_EXCL | O_TRUNC | O_RDONLY | O_RDWR
//mode : S_IRUSR|S_IWUSR
//return : file descriptor
int shm_open_(const char *name, int oflag, mode_t mode ){
    int fd = shm_open(name, oflag, mode);
    if (fd == -1){
        perror("failed to open shared memory");
        exit(1);
    }
    return fd;
}

void ftruncate_(int fd, off_t length){
    if (ftruncate(fd, length) == -1){
        perror("failed ftruncate");
        exit(1);
    }    
}

//addt : NULL
//length : ftruncate length
//prot : PROT_READ | PROT_WRITE
//flags :  MAP_SHARED
//offset : 0
void *mmap_(void *addr, size_t length, int prot, int
flags,int fd, off_t offset){
    void *p = mmap(addr, length, prot, flags,fd, offset);
    if (p == MAP_FAILED){
        perror("failed mmap");
        exit(1);
    }
    return p;
}

void munmap_(void *addr, size_t length){
    if (munmap(addr, length) == -1){
        perror("failed munmap");
        exit(1);
    }
}

void close_(int fd){
    if (close(fd) == -1){
        perror("failed closing file descriptor");
        exit(1);
    }
}

void shm_unlink_(const char *name){
    if (shm_unlink(name) == -1){
        perror("failed shm_unlink");
        exit(1);
    }
}


/*SEMAPHORES*/

//oflag : O_CREAT | O_EXCL | O_TRUNC | O_RDONLY | O_RDWR
//mode : S_IRUSR|S_IWUSR
sem_t* sem_open_(char *name, int oflag, mode_t mode, unsigned int value){
    sem_t *sem = sem_open(name, oflag, mode, value);

    if (sem == SEM_FAILED){
         perror("Semaphore failed"); 
         exit(1); 
    }
    return sem;
}

void sem_wait_(sem_t *sem){
    if (sem_wait(sem) == -1){
        perror("failed sem_wait()"); 
        exit(1);
    }
}

void sem_post_(sem_t *sem){
    if (sem_post(sem) == -1){
        perror("failed sem_wait()"); 
        exit(1);
    }
}

void sem_getvalue_(sem_t *sem, int *sval){
    if  (sem_getvalue(sem, sval) == -1){
        perror("failed sem_getvalue");
        exit(1);
    }
}

void sem_unlink_(char *name){
    if (sem_unlink(name) == -1){
        perror("Failed unlinking semaphore");
        exit(1);
    }
}

void sem_close_(sem_t *sem){
    if (sem_close(sem)==-1) {
        perror("sem_close failure.");
        exit(1);
    }
}

/*OUTROS*/

void print(char * s){
    printf("%s", s);
    fflush(stdout);
}

int spawn_childs(int n){
    pid_t p; /* pid of child process */
    int i;

    /* creates n child processes that return it's order. Parent returns zero. */
    for (i = 1; i <= n; i++){

        p = fork_();
        if (p == 0){
            return(i);
        } else if (p == -1){
            return -1;
        }

    }
    return(0);
}

int is_child(pid_t p){
    return p == 0;
}