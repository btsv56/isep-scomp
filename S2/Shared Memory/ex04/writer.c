/*
* Author: Bruno Ribeiro - 1180573
* Revised by: Bruno Veiga - 1180712
* Revised by: João Leal - 1180723
*/

#include <stdio.h>
#include "../share/functions.h"
#include "shared_struct.h"

int main() {

    /*Declaração das Variáveis*/
    int fd;
    int data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    int i;

    /*Criar zona de Mamória Partilhada*/
    fd = shm_open_(SHARED_NAME, O_CREAT |O_EXCL| O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, data_size);
    shared_data = mmap_(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*Desenvolvimento*/
    srand(time(NULL));

    for (i = 0; i < SIZE; i++){
        shared_data->chars[i] = rand() % 26 + 65;
    }
	
    /*Fecha zona de Memória Partilhada do Processo*/
	munmap_(shared_data, data_size);
	close_(fd);

    return 0;
}