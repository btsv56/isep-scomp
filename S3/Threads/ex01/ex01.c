/**
 * Author: João Leal©
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
#include <pthread.h>

void* thread_func(void*arg) {
    printf("I am thread %u and I am executing\n", pthread_self());
    pthread_exit((void*)NULL);
}

/**
 * a) 6 processes.
 * b) 2 threads.
 */

int main() {                                                    //main process (processes: 1)
    pid_t pid2, pid3;
    pid_t pid = fork();                                         //one process is created (processes: 2)
    
    if (pid == 0) {                                             //only child enters
        pid2=fork();                                            //one process created (processes: 3)
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, thread_func, NULL);    //thread created by two processes (threads: 2)
        pthread_join(thread_id, NULL);
    }

    pid3=fork();                                                //three processes created (processes: 6)
    printf("I am process %d\n", getpid());
    if (pid==0 || pid2==0 || pid3==0) exit(0);
    while(wait(NULL)>0);

    return 0;
}