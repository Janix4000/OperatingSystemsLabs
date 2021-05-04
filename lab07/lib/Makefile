CC = gcc
CFLAGS = -Wall -O0 -g

ARFLAGS = rcs

static: clean libipc.a

libipc.a: sem_static.o
	$(AR) $(ARFLAGS) libipc.a sem.o

sem_static.o:
	$(CC) $(CFLAGS) -c sem.c 

clean:
	$(RM) -f *.o *.a *.so