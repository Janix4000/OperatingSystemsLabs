CC = gcc
CFLAGS = -Wall -O0 -g

ARFLAGS = rcs

static: clean libvector.a


libvector.a: vector_static.o
	$(AR) $(ARFLAGS) libvector.a client_vector.o

vector_static.o:
	$(CC) $(CFLAGS) -c client_vector.c 

clean:
	$(RM) -f *.o *.a *.so