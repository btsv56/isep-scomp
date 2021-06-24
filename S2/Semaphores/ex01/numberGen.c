/**
 * Author: João Leal©
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Creates 1600 random numbers between 0 and 5000 and writes them in input.txt
 */

int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        if (n > RAND_MAX) return -1;

        int end = RAND_MAX / n;
        if (end <= 0) return -1;
        end *= n;

        int r;
        while ((r = rand()) >= end);
        return r % n + 1;
    }
}

int main() {
    int i;
    srand(time(NULL));
    FILE *input;
    input=fopen("input.txt", "w");
    for (i=0;i<1600;i++) {
        fprintf(input, "%d\n", randint(5001)-1);
    }
    fclose(input);
    return 0;
}