/*
* Author: Bruno Ribeiro - 1180573
* Revised by: Bruno Veiga - 1180712
* Revised by: João Leal - 1180723
*/

#include <stdio.h>
#include "../share/functions.h"
#include "shared_struct.h"

int main() { 

    /*Declaração de Variáveis*/
    int fd;
    int data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    int i;
    int sum = 0;
    int avg;
    char c;

    /*Criação da Memória Partilhada*/
    fd = shm_open_(SHARED_NAME, O_EXCL|O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, data_size);
    shared_data = mmap_(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


    /*Desenvolvimento*/
    for (i = 0; i < SIZE; i++){
        c = shared_data->chars[i];
        sum += c;
        printf("%d: %c\n", i + 1, c);
    }
    avg = sum/SIZE;
    printf("\n\nAverage: %c|%hhd\n\n", avg, avg);


    /*Remove a Memória Partilhada do Sistema de Ficheiros*/
	munmap_(shared_data, data_size);
    close_(fd);
	shm_unlink_(SHARED_NAME);

    return 0;
}