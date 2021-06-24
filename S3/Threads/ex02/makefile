ex02: ex02.o
	gcc -g -Wall ex02.o -o ex02 -lpthread

ex02.o:ex02.c
	gcc -g -Wall -c ex02.c -o ex02.o -lpthread
	
run: ex02
	./ex02
	
clean:
	rm *.o ex02