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
#include <time.h>

#define VEC_SIZE_PARENT 1000
#define NUMBER_PRC 4
#define CHLD_PRCL VEC_SIZE_PARENT/NUMBER_PRC

/**
 * Author: Bruno Veiga
 *
 * Revised by: Bruno Veiga
 * Revised by: João Leal - 1180723
 * Revised by: Bruno RIbeiro - 1180573
 */

typedef struct {
    int maxs[10];
    int new_data;
} Local_maxs;

/* places random numbers (0-1000) in an array */
void randArray(int *vec) {
    int i=0, num;
    while (i++<VEC_SIZE_PARENT) {
        num=rand()%1000;
        *(vec+i)=num;
    }
}

int main() {
    int vec[VEC_SIZE_PARENT]={0}, fd, data_size=sizeof(Local_maxs), i, status;
    Local_maxs *local_maxs;
    pid_t pid;
    clock_t start, end;

    srand(time(NULL));
    randArray(vec);

    fd = shm_open("/shm_ex09", O_CREAT| O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    local_maxs=(Local_maxs *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (local_maxs==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    local_maxs->new_data=1;

    for (i=0;i<NUMBER_PRC;i++) {
        if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
        start=clock();
        if (pid==0) {

            int max=0;
            int pos=0;
            for (pos=0+i*CHLD_PRCL;pos<CHLD_PRCL*i+CHLD_PRCL;pos++) {
                if (vec[pos]>max) max=vec[pos];
            }
            printf("(%d-%d) Max: %d\n", 0+i*CHLD_PRCL, CHLD_PRCL*i+CHLD_PRCL-1, max);

            while (!local_maxs->new_data);
            local_maxs->new_data=0;

            local_maxs->maxs[i]=max;

            local_maxs->new_data=1;
			
			if(munmap(local_maxs, data_size)==-1) { perror("Munmap failure."); exit(1); }
			if(close(fd) ==-1) {perror("Close failure\n");exit(1);}

            exit(0);
        }
    }

    for(i=0;i<NUMBER_PRC;i++){
    	 wait(&status);
    }
    int glo_max=0;
    for (i=0;i<NUMBER_PRC;i++) {
        if (local_maxs->maxs[i]>glo_max) {
            glo_max=local_maxs->maxs[i];
        }
    }
    end=clock();
    printf("Global max: %d\n", glo_max);
    printf("Tempo %d processos: %.6f\n",NUMBER_PRC, (float) (end - start) / CLOCKS_PER_SEC);
    if(munmap(local_maxs, data_size)==-1) { perror("Munmap failure."); exit(1); }
	if(close(fd) ==-1) {perror("Close failure\n");exit(1);}
    if (shm_unlink("/shm_ex09")==-1) { perror("Closing failure."); exit(1); }
    return 0;
}
