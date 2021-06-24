all: main
	
main.o: main.c
	gcc -Wall -g -c main.c -lpthread -lrt
	
main: main.o
	gcc -Wall -g main.o -o main -lpthread -lrt
	
run: all
	./main

clean:
	rm -rf *.o main