ex11.o: ex11.c
	gcc -Wall -g -c ex11.c -lrt
	
ex11: ex11.o
	gcc -Wall -g ex11.o -o ex11 -lrt
	
run: ex11
	./ex11

clean:
	rm -rf *.o ex11