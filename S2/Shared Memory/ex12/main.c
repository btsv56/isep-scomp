/*
* Autor: Bruno Ribeiro - 1180573
* Revised by: Bruno Veiga - 1180712
* Revised by: João Leal - 1180723
*/

#include "../share/functions.h"
#include "shared_struct.h"


int main() {
    /*Declaração de Variáveis*/
    int fd;
    int data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    pid_t p;
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int i;
    int highest_grade;
    int lowest_grade;
    int sum = 0;
    int avg_grade;

    /*Criação de Memória Partilhada*/
    fd = shm_open_(SHARED_NAME, O_CREAT |O_EXCL| O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate_(fd, data_size);
    shared_data = mmap_(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    /*Inicialização da estrutura de dados partilhado*/
    shared_data->new_data = 0; //esta a zero para o filho nao avancar ate o pai escrever tudo

    p = fork_();

    /*Processo Pai*/
    if (p > 0){
        printf("Escreva o numero do aluno:\n");
        scanf("%d",&numero);

        printf("Escreva o nome do aluno:\n");
        scanf("%s",nome);

        for (i = 0; i < NR_DISC; i++){
            printf("Escreva a nota da disciplina %d do aluno:\n", i + 1);
            while ((getchar())!='\n');
            scanf("%d",&disciplinas[i]);
        }

        shared_data->a.numero = numero;
        strcpy(shared_data->a.nome, nome);
        for (i = 0; i < 10; i++){
            shared_data->a.disciplinas[i] = disciplinas[i];
        }
        shared_data->new_data = 1;

        munmap_(shared_data, data_size);
        close_(fd);
        wait(NULL);
    }

    /*Processo Filho*/
    if (p == 0){
        while(!shared_data->new_data); //espera ativa ate o processo pai termniar de preenchar zona critica

        lowest_grade = shared_data->a.disciplinas[0];
        highest_grade = shared_data->a.disciplinas[0];
        sum += shared_data->a.disciplinas[0];

        for(i = 1; i < NR_DISC; i++){
            sum += shared_data->a.disciplinas[i];
            if (shared_data->a.disciplinas[i] > highest_grade){
                highest_grade = shared_data->a.disciplinas[i];
            }
            if (shared_data->a.disciplinas[i] < lowest_grade){
                lowest_grade = shared_data->a.disciplinas[i];
            }
        }
        avg_grade = sum / NR_DISC;

        printf("Highest grade: %d\n", highest_grade);
        printf("Lower grade: %d\n", lowest_grade);
        printf("Average: %d\n", avg_grade);

        munmap_(shared_data, data_size);
        close_(fd);
        exit(0);
    }

    /*Libertação da Memória Partilhada no Sistema de Ficheiros*/
	shm_unlink_(SHARED_NAME);
    return 0;
}

