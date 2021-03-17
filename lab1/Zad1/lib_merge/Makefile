CC = gcc
CFLAGS = -Wall -O0 -g

static: clean
	$(CC) $(CFLAGS) -c lib_vector.c -o lib_vector.o 
	$(CC) $(CFLAGS) -c lib_merge.c -o lib_merge.o 
	ar rcs libmerge.a lib_merge.o lib_vector.o

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so