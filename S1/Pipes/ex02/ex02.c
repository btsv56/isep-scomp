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
#include <string.h>

#define BUFFER_SIZE 80

typedef struct {
    int a;
    char b[BUFFER_SIZE];
} structb;

void maina() {
    int write_int, read_int, fd[2];
    char write_str[BUFFER_SIZE], read_str[BUFFER_SIZE];
    pid_t pid;

	printf("\na)\n");

    if (pipe(fd)==-1) {
        perror("Pipe failed");
        exit(1);
    }

    if ((pid=fork())==-1) { perror("Fork failed"); exit(1); }

    if (pid>0) {
        close(fd[0]);

        /* scans integer and string and writes them separately */
        printf("Integer: ");
        scanf("%d", &write_int);
        while ((getchar()) != '\n');
        write(fd[1], &write_int, 4);
        printf("String: ");
        fgets(write_str, BUFFER_SIZE, stdin);
        write(fd[1], write_str, strlen(write_str)+1);

        close(fd[1]);
    } else {
        close(fd[1]);

        /* reads integer and string separately */
        read(fd[0], &read_int, 4);
        read(fd[0], read_str, BUFFER_SIZE);
        printf("Integer: %d, String: %s\n", read_int, read_str);
        close(fd[0]);
    }
}

void mainb() {
    int fd[2];
    structb bs, br;
    structb *bsp, *brp;
    pid_t pid;
    
    printf("\nb)\n");

    if (pipe(fd)==-1) {
        perror("Pipe failed");
        exit(1);
    }

    bsp=&bs;
    brp=&br;

    if ((pid=fork())==-1) { perror("Fork failed"); exit(1); }

    if (pid>0) {
        close(fd[0]);
        /* scans integer and string and puts them in the struct, followed by only writing the struct once */
        printf("Integer: ");
        scanf("%d", &(bsp->a));
        while ((getchar()) != '\n');
        printf("String: ");
        fgets(bsp->b, BUFFER_SIZE, stdin);
        write(fd[1], bsp, 84);

        close(fd[1]);
    } else {
        close(fd[1]);

        /* reads the struct and prints the integer and string from it */
        read(fd[0], brp, 84);
        printf("Integer: %d, String: %s\n", brp->a, brp->b);
        close(fd[0]);
    }
}

int main() {
    char version;
    printf("Choose a) or b)\n");

    do {
    	scanf("%c", &version);
    	fflush(stdin);
    	if (version=='a') maina();
    	else if (version=='b') mainb();
    } while (version!='a' && version!='b');
    return 0;
}
