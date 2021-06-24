/*
 * ex02.c
 *
 *  Created on: 15/05/2020
 *      Author: isep
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS 5
 
 typedef struct {
	int number;
	char name[20];
	int grade;
 }s;
 
void* thread_func(void *arg) {
	s *s_temp=((s*)arg);
	printf("Number: %d\nName: %s\nGrade: %d\n\n",s_temp->number,s_temp->name,s_temp->grade);
	pthread_exit((void*)NULL);
}
 
int main() {
	pthread_t threads[THREADS];
	s array[THREADS];
	int i;
	for(i=0;i<THREADS;i++) {
		array[i].number=rand()%10;
		sprintf(array[i].name,"Name %d",i);
		array[i].grade=rand()%20;
	}
	for(i=0;i<THREADS;i++) {
		if(pthread_create(&threads[i],NULL, thread_func,(void*) &array[i])!=0) {perror("Erro a criar thread\n"); exit(1);}
		if(pthread_join(threads[i],NULL)!=0) {perror("Erro a dar join da thread\n");exit(1);}
	}
	exit(0);
}

