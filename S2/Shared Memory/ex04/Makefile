all: writer reader

reader.o: reader.c
	gcc -Wall -g -c reader.c -lpthread -lrt
	
reader: reader.o
	gcc -Wall -g reader.o -o reader -lpthread -lrt
	
writer.o: writer.c
	gcc -Wall -g -c writer.c -lpthread -lrt
	
writer: writer.o
	gcc -Wall -g writer.o -o writer -lpthread -lrt
	
run: all
	./writer
	./reader

clean:
	rm -rf *.o writer reader