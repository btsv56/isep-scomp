ex03: ex03.o
	gcc -Wall -g ex03.o -o ex03 -lrt -lpthread

ex03.o: ex03.c
	gcc -Wall -g -c ex03.c -lrt -lpthread
		
run: ex03
	./ex03

clean:
	rm -rf *.o ex03