ex09.o: ex09.c
	gcc -Wall -g -c ex09.c -lrt
	
ex09: ex09.o
	gcc -Wall -g ex09.o -o ex09 -lrt
	
run: ex09
	./ex09

clean:
	rm -rf *.o ex09