CC = gcc
CFLAGS = -Wall -O0 -std=c11

static:
	$(CC) $(CFLAGS) -c lib_merge.c -o lib_merge.o 
	ar rcs lib_merge.a lib_merge.o 

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so
	rm -f *merged*

main1: clean static
	$(CC) $(CFLAGS) -c main.c -o main.o
	$(CC) $(CFLAGS) -static main.o -L. lib_merge.a -o main
	make run

run:
	./main
