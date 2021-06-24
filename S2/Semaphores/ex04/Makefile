ex04: ex04.o
	gcc -Wall -g ex04.o -o ex04 -lrt -lpthread

ex04.o: ex04.c
	gcc -Wall -g -c ex04.c -lrt -lpthread
		
run: ex04
	./ex04

clean:
	rm -rf *.o ex04