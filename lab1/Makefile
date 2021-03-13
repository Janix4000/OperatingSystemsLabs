CC = gcc
CFLAGS = -Wall -O0 -std=c11

static:
	$(CC) $(CFLAGS) -c library.c -o library.o 
	ar rcs library.a library.o 

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so
	rm -f *merged*

main1: clean static
	$(CC) $(CFLAGS) -c main.c -o main.o
	$(CC) $(CFLAGS) -static main.o -L. library.a -o main
	make run

run:
	./main
