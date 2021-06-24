all: ex14.o
	gcc -g -Wall ex14.o -o ex14 -lrt
	
ex14.o: ex14.c
	gcc -g -Wall -c ex14.c -o ex14.o -lrt
	
run: all
	./ex14
	
clean:
	rm -rf *.o ex14