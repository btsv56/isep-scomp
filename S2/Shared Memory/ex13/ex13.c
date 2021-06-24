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

/**
 * Author: João Leal
 * 
 * Revised by: Bruno Veiga - 1180712
 * Revised by: Bruno Ribeiro - 1180573
 */

typedef struct {
    char path[80];
    char word[80];
    int ocurrences;
} Search_struct;

typedef struct {
    Search_struct structs[10];
} Mem_struct;

int main() {
    int fd, data_size=sizeof(Mem_struct), i, status;
    Mem_struct * mem_struct;
    pid_t pid;
    FILE * input;

    fd = shm_open("/shm_ex13", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    mem_struct=(Mem_struct *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem_struct==MAP_FAILED) { perror("Mmap failure."); exit(1); }

    /*create_structs(mem_struct);*/
    Search_struct * structs=mem_struct->structs;
    
    strcpy(structs[0].path,"./child1.txt");
    strcpy(structs[1].path,"./child2.txt");
    strcpy(structs[2].path,"./child3.txt");
    strcpy(structs[3].path,"./child4.txt");
    strcpy(structs[4].path,"./child5.txt");
    strcpy(structs[5].path,"./child6.txt");
    strcpy(structs[6].path,"./child7.txt");
    strcpy(structs[7].path,"./child8.txt");
    strcpy(structs[8].path,"./child9.txt");
    strcpy(structs[9].path,"./child10.txt");

    strcpy(structs[0].word,"reject");
    strcpy(structs[1].word,"your");
    strcpy(structs[2].word,"sense");
    strcpy(structs[3].word,"of");
    strcpy(structs[4].word,"injury");
    strcpy(structs[5].word,"and");
    strcpy(structs[6].word,"the");
    strcpy(structs[7].word,"injury");
    strcpy(structs[8].word,"itself");
    strcpy(structs[9].word,"disappears");

    for (i=0;i<10;i++) {
        char temp[80];
        int counter=0;
        if ((pid=fork())==-1) { perror("Fork failure."); exit(1); }
        if (pid==0) {
            Search_struct search=mem_struct->structs[i];
            
            input=fopen(search.path, "r");
            if (!input) { perror("Could not open file."); exit(1); }

            while (fscanf(input, "%s", temp)==1) {
                if (strcmp(temp, search.word)==0) {
                    counter++;
                }
            }
            mem_struct->structs[i].ocurrences=counter;
            if (munmap(mem_struct, data_size)==-1) { perror("Munmap failure."); exit(1); }
            if (close(fd)==-1) { perror("Closing failure."); exit(1); }

            exit(0);
        }
    }

    for (i=0;i<10;i++) {
        wait(&status);
    }

    for (i=0;i<10;i++) {
        printf("In %s there are %d ocurrences of the word %s\n", structs[i].path, structs[i].ocurrences, structs[i].word);
    }

    if (munmap(mem_struct, data_size)==-1) { perror("Munmap failure."); exit(1); }
    
    if (close(fd)==-1) { perror("Closing failure."); exit(1); }

    if (shm_unlink("/shm_ex13") < 0) { perror("Unlink failure."); exit(1);}

    return 0;
}
