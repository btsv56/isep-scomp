ex13.o: ex13.c
	gcc -Wall -g -c ex13.c -lrt
	
ex13: ex13.o
	gcc -Wall -g ex13.o -o ex13 -lrt
	
run: ex13
	./ex13

clean:
	rm -rf *.o ex13