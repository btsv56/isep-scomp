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

#define CHLD_NUM 8

int main() {
    int i, j=0, line=0, tmp, out;
    char name[20];
	pid_t pid;
    sem_t *sems[CHLD_NUM];
    FILE *input, *output;

    /* clears output.txt */
    output=fopen("./output.txt", "w");
    fprintf(output, " ");
    fclose(output);

    /* opens first semaphore, starting "green" */
    if ((sems[0] = sem_open("number1", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }

    /* opens the other semaphores, starting "red" */
    for (i=1;i<CHLD_NUM;i++) {
        sprintf(name, "number%d", i+1);
        if ((sems[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) { perror("Semaphore failure."); exit(1); }
    }

    for (i=0;i<CHLD_NUM;i++) {
        /* creates childs */
        if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
        if (pid==0) {
            if (sem_wait(sems[i])==-1) { perror("Semaphore 'wait' failure."); exit(1); } /* decrements semaphore of respective child (child 1 will start first) */
            input=fopen("./input.txt", "r"); /* opens .txts */
            output=fopen("./output.txt", "a");
            
            for (j=i*200;j<(i+1)*200;j++) {
                while (line!=i*200) {
                    fscanf(input, "%d", &tmp); /* skips lines */
                    line++;
                }
                fscanf(input, "%d", &tmp); /* reads number from input.txt */
                fprintf(output, "%d\n", tmp); /* writes number on output.txt */
            }
            fclose(input);
            fclose(output);
            if (sem_post(sems[i+1])==-1) { perror("Semaphore 'post' failure."); exit(1); } /* increments semaphore of next child */
            exit(0);
        }
    }

    while (wait(NULL)>0); /* waits for childs */
    output=fopen("./output.txt", "r");
    while (fscanf(output, "%d", &out) != EOF) { /* prints numbers from output.txt */
		printf("%d\n", out);
	}
    fclose(output);
    
    /* closes semaphores */
    for (i = 0; i < CHLD_NUM; i++){
		sprintf(name, "number%d", i+1);
		sem_unlink(name);
	}
	return 0;
}