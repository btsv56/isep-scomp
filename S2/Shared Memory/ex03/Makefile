all: ex03 -lrt
	
ex03.o:	ex03.c
	gcc -Wall -g -c ex03.c -o ex03.o -lrt
	
ex03: ex03.o
	gcc -Wall -g ex03.o -o ex03 -lrt
	
run:
	./ex03
	
clean:
	rm -rf *.o ex03 