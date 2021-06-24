all: seller client1 client2 client3 client4 client5

seller: seller.o
	gcc -Wall -g seller.o -o seller -lrt -lpthread

seller.o: seller.c
	gcc -Wall -g -c seller.c -lrt -lpthread
	
client1: client1.o
	gcc -Wall -g client1.o -o client1 -lrt -lpthread	

client1.o: client1.c
	gcc -Wall -g -c client1.c -lrt -lpthread
	
client2: client2.o
	gcc -Wall -g client2.o -o client2 -lrt -lpthread	

client2.o: client2.c
	gcc -Wall -g -c client2.c -lrt -lpthread
	
client3: client3.o
	gcc -Wall -g client3.o -o client3 -lrt -lpthread	

client3.o: client3.c
	gcc -Wall -g -c client3.c -lrt -lpthread
	
client4: client4.o
	gcc -Wall -g client4.o -o client4 -lrt -lpthread	

client4.o: client4.c
	gcc -Wall -g -c client4.c -lrt -lpthread
	
client5: client5.o
	gcc -Wall -g client5.o -o client5 -lrt -lpthread	

client5.o: client5.c
	gcc -Wall -g -c client5.c -lrt -lpthread	
			
run: all
	./seller

clean:
	rm -rf *.o seller client