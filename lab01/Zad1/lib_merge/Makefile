CC = gcc
CFLAGS = -Wall -O0 -g

ARFLAGS = rcs

all: clean libmerge.a libmerge.so

static: clean libmerge.a
shared: clean libmerge.so

libmerge.so:
	$(CC) $(CFLAGS) lib_merge.c lib_vector.c -fPIC -shared -o libmerge.so

libmerge.a: lib_merge_static.o lib_vector_static.o
	$(AR) $(ARFLAGS) libmerge.a lib_merge_static.o lib_vector_static.o

lib_merge_static.o:
	$(CC) $(CFLAGS) -c lib_merge.c -o lib_merge_static.o 

lib_vector_static.o:
	$(CC) $(CFLAGS) -c lib_vector.c -o lib_vector_static.o 

clean:
	$(RM) -f *.o *.a *.so