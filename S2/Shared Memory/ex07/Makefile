all: writer reader

reader.o: reader.c
	gcc -Wall -g -c reader.c -lrt
	
reader: reader.o
	gcc -Wall -g reader.o -o reader -lrt
	
writer.o: writer.c
	gcc -Wall -g -c writer.c -lrt
	
writer: writer.o
	gcc -Wall -g writer.o -o writer -lrt
	
run: all
	./writer
	./reader

clean:
	rm -rf *.o writer reader