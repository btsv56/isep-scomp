#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "student.h"

/**
 * Author: João Leal
 * Revised by: Bruno Veiga - 1180712
 *             Bruno Ribeiro - 1180573
 */

int main() {
    int fd, data_size=sizeof(Student);
    Student *student;

    fd=shm_open(shm_name, O_EXCL|O_RDWR, S_IRUSR | S_IWUSR);
    if (fd==-1) { perror("Shared memory failure."); exit(1); }

    if (ftruncate(fd, data_size)==-1) { perror("Size allocation failure."); exit(1); }

    student=(Student *) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (student == MAP_FAILED) { perror("Mmap failure."); exit(1); }

    printf("Student number: %d\nStudent name: %s\n", student->number, student->name);
	if (munmap(student, data_size) < 0) {  /* Fecho da zona de memoria partilhada */
		perror("Erro no munmap\n");
		exit(1);
	}
    if (close(fd) == -1) {
		perror("Error no close\n");
		exit(1);
	}
	if (shm_unlink(shm_name) == -1) {
		perror("Erro no shm_unlink\n");
		exit(1);
	}
    return 0;
}