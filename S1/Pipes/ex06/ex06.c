/*
 * Author: João Leal - 1108723
 * Revised by:
 * 		- Bruno Veiga - 1180712
 * 		- Bruno Ribeiro - 1180573
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define VEC_SIZE 1000
#define CHLD_SCTR VEC_SIZE/5

/* places 5 random numbers (1-10) in 5 random positions (0-999) in both arrays and prints them */
void randArrays(int *vec1, int *vec2) {
    int i=0, pos, num;
    while (i++<5) {
        pos=rand()%VEC_SIZE;
        num=rand()%10+1;
        *(vec1+pos)=num;
        printf("vec1[%d] = %d\n", pos, num);
    }
    i=0;
    while (i++<5) {
        pos=rand()%VEC_SIZE;
        num=rand()%10+1;
        *(vec2+pos)=num;
        printf("vec2[%d] = %d\n", pos, num);
    }
}

int main() {
    int i=0, vec1[VEC_SIZE]={0}, vec2[VEC_SIZE]={0} /* arrays are initialized with 0s */, fd[2], tmp=0, tmpw, sum=0;
    pid_t pid;

    /* change rand() seed */
    srand(time(NULL));
    randArrays(vec1, vec2);

    if (pipe(fd)==-1) {
        perror("Pipe error");
        return -1;
    }

    /* creates 5 child processes */
    while (i<5) {
        pid=fork();
        if (pid==-1) {perror("Fork failed"); return -1;}
        else if (pid==0) {
            break;
        }
        i++;
    }

    if (pid>0) {
        close(fd[1]);
        i=0;
        /* reads partial sum 5 times and adds it to total sum */
        while (i++<5) {
            read(fd[0], &tmp, 4);
            sum+=tmp;
        }
        close(fd[0]);
        printf("Sum = %d\n", sum);
    } else {
        int x=0;
        tmpw=0;
        /* sums child's respective sector */
        for (x=0+CHLD_SCTR*i;x<CHLD_SCTR*i+CHLD_SCTR;x++) {
            tmpw+=(vec1[x]+vec2[x]);
        }
        close(fd[0]);
        /* writes result of sum */
        write(fd[1], &tmpw, sizeof(tmpw));
        close(fd[1]);
    }

    return 0;
}
