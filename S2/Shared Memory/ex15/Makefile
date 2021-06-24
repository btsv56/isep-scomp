all: ex15.o
	gcc -g -Wall ex15.o -o ex15 -lrt
	
ex15.o: ex15.c
	gcc -g -Wall -c ex15.c -o ex15.o -lrt
	
run: all
	./ex15
	
clean:
	rm -rf *.o ex15