all: consult insert consult_all clear_mem

ui: ui.c
	gcc -Wall -g ui.c -o ui -lrt -lpthread

consult: consult.c
	gcc -Wall -g consult.c -o consult -lrt -lpthread
	
consult_all: consult_all.c
	gcc -Wall -g consult_all.c -o consult_all -lrt -lpthread	
	
insert: insert.c
	gcc -Wall -g insert.c -o insert -lrt -lpthread

clear_mem: clear_mem.c
	gcc -Wall -g clear_mem.c -o clear_mem -lrt -lpthread
	
run_insert: insert
	./insert

run_consult: consult
	./consult
	
run_consult_all: consult_all
	./consult_all

run_clear_mem: clear_mem
	./clear_mem

run: ui all
	./ui

clean:
	rm -rf consult insert consult_all clear_mem