numGen: numberGen
	./numberGen
	
ex01.o: ex01.c
	gcc -Wall -g -c ex01.c -lpthread

ex01: ex01.o
	gcc -Wall -g ex01.o -o ex01 -lpthread

run: numGen ex01
	./ex01