all: ex06.o
	gcc -g -Wall ex06.o -o ex06 -lrt
	
ex06.o: ex06.c
	gcc -g -Wall -c ex06.c -o ex06.o -lrt
	
run: all
	./ex06
	
clean:
	rm -rf *.o ex06