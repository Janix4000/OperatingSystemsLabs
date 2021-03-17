CC = gcc
CFLAGS = -Wall -O0
LDLIBS = -lmerge

main: clean
	# $(CC) $(CFLAGS) -c  $(LDLIBS) main.c -o main.o
	$(CC) $(CFLAGS) -static -L../lib_merge -I../lib_merge main.c $(LDLIBS) -o main
	make run

run:
	./main

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so