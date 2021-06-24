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
#define NUMBER_PRC 6
#define CHLD_PRCL VEC_SIZE_PARENT/NUMBER_PRC

/**
 * Author: Pedro Brandao
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: João Leal - 1180723
 * Revised by: Bruno Ribeiro - 1180573
 */

/* places random numbers (0-1000) in an array */
void randArray(int *vec) {
    int i=0, num;
    while (i++<VEC_SIZE_PARENT) {
        num=rand()%1001;
        *(vec+i)=num;
    }
}

int main() {
    int vec[VEC_SIZE_PARENT]={0}, i, status;
    pid_t pid;

    int fd[2];

    srand(time(NULL));
    randArray(vec);

    if(pipe(fd) == -1){
    	perror("Pipe Creation failure\n");
    	exit(1);
    }

    for (i=0;i<NUMBER_PRC;i++) {

        if ((pid=fork())==-1) {
        	perror("Fork failure.");
        	exit(1);
        }

        if (pid==0) {

            int max=0;
            int pos=0;
            for (pos=0+i*CHLD_PRCL;pos<CHLD_PRCL*i+CHLD_PRCL;pos++) {
                if (vec[pos]>max)
                	max=vec[pos];
            }

            printf("(%d-%d) Max: %d\n", 0+i*CHLD_PRCL, CHLD_PRCL*i+CHLD_PRCL-1, max);

            if(close(fd[0])==-1){perror("Erro no close\n");exit(1);}
            if(write(fd[1],&max,sizeof(max))==-1){perror("Erro no write\n");exit(1);}
            if(close(fd[1])==-1){perror("Erro no close\n");exit(1);}

            exit(0);
        }
    }



    int glo_max=0;
    int max;
    if(close(fd[1])==-1){perror("Erro no close\n");exit(1);}
    while(read(fd[0],&max,sizeof(int))){
    	if(glo_max<max){
    		glo_max = max;
    	}
    }
    if(close(fd[0])==-1) {perror("Erro no close\n");exit(1);}

    for(i=0;i<NUMBER_PRC;i++){
    	wait(&status);
    }


    printf("Global max: %d\n", glo_max);

    return 0;
}
