#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

/**
 * Author: João Leal
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

typedef struct {
	int int1;
	int int2;
    int new_data;
} Ints;

int main() {
    int fd, data_size=sizeof(Ints), i=0, status=1;
    pid_t pid;
    Ints *ints;

    fd = shm_open("/shm_ex05", O_CREAT |O_EXCL| O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    ints=(Ints *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ints==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    ints->int1=8000;
    ints->int2=200;
    ints->new_data=0;

    if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
    if (pid==0) {
        while (!ints->new_data);
        for (i=0;i<1000000;i++) {
            ints->int1-=1;
            ints->int2+=1;
        }
        if(munmap(ints, data_size)==-1) { perror("Munmap failure."); exit(1); }
        if (close(fd)==-1) { perror("Closing failure."); exit(1); }
        exit(0);
    } else {
        for (i=0;i<1000000;i++) {
            ints->int1+=1;
            ints->int2-=1;
        }
        ints->new_data=1;
    }

    wait(&status);
    if(WIFEXITED(status) && WEXITSTATUS(status)!=255 ) {
        printf("First number: %d\nSecond number: %d\n", ints->int1, ints->int2); /* The numbers will always be correct (8000 and 200) because there 
        is a synchronization method being used, if there wasn't, the number would be overwritten, and would vary wildly every run*/
    }

    if(munmap(ints, data_size)==-1) { perror("Munmap failure."); exit(1); }
    
    if (close(fd)==-1) { perror("Closing failure."); exit(1); }

    if(shm_unlink("/shm_ex05") == -1) { perror("error unlink shared memory"); exit(1); }

    return 0;
}