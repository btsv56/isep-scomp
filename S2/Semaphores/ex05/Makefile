ex05: ex05.o
	gcc -Wall -g ex05.o -o ex05 -lrt -lpthread

ex05.o: ex05.c
	gcc -Wall -g -c ex05.c -lrt -lpthread
		
run: ex05
	./ex05

clean:
	rm -rf *.o ex05