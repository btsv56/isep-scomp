#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 1000
#define THREADS 10
#define VALUE 20

int vec[SIZE];

typedef struct{
    int id;
} struct_winner;

void *thread_func(void *arg){
    int i;
    int id = *((int *)arg);
    for (i = id*100; i < (id*100) + 100; i++){
        if (vec[i] == VALUE){
			struct_winner *winner = (struct_winner *)malloc(sizeof(struct_winner));
			winner->id = id;
            printf("Valor: %d\nPosição: %d\n", VALUE, i);
            pthread_exit((void *)winner);
        }
    }
    pthread_exit((void*)NULL);
}

int main(){
    int array[THREADS],i;
	struct_winner winner_s;
    void *winner;
    pthread_t threads[THREADS];
	
	for (i = 0; i < SIZE; i++){
        vec[i] = i;
    }
	
    for (i = 0; i < THREADS; i++){
        array[i] = i;
        if(pthread_create(&threads[i], NULL, thread_func, (void *)&array[i])!=0) {perror("Erro no pthread_create\n");exit(1);}
		if(pthread_join(threads[i], (void *)&winner)!=0) {perror("Erro no pthread_join\n");exit(1);}
        if ((struct_winner *)winner != NULL){
            winner_s = *((struct_winner *)winner);
            printf("Thread vencedora: %d\n", winner_s.id);
        }
    }
	
    free(winner);

    return 0;
}