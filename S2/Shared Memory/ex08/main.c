/**
 * Autor: Bruno Ribeiro - 1180573
 * Revised by: Bruno Veiga- 1180712
 */
#include "../share/functions.h"
#include "shared_struct.h"

/* Resposta: Não devido ao facto de partilharem a memoria, ou seja, como eles fazem as operacoes ao mesmo numero mas a timings diferentes
 * os resultados nunca vão ser os teoricamente esperados numa situação normal.*/

void operation(shared_data_type *shared_data, int n_times)
{
    int i;
    for (i = 0; i < n_times; i++)
    {
        shared_data->n++;
        shared_data->n--;
    }
}

int main()
{
    /*Declaração de Variáveis*/
    int fd;
    int data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    /*Criação de Memória Partilhada*/
    fd = shm_open_(SHARED_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, data_size);
    shared_data = mmap_(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    /*Inicialização dos dados da Memória Partilhada*/
    shared_data->n = 100;

    /*Processo Filho*/
    if (fork_() == 0)
    {
        operation(shared_data, SIZE);

        munmap_(shared_data, data_size);
        close_(fd);
        exit(0);
    } 

    /*Processo Pai*/
    operation(shared_data, SIZE);

    wait_(NULL);

    printf("Result: %d\n", shared_data->n);

    /*Libertação de Memória Partilhada*/
    munmap_(shared_data, data_size);
    close_(fd);
    shm_unlink_(SHARED_NAME);

    return 0;
}