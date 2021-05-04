CC = gcc
CFLAGS = -Wall -O0 -g

ARFLAGS = rcs

static: clean libipc.a

libipc.a: sem_static.o shm_static.o
	$(AR) $(ARFLAGS) libipc.a sem.o shm.o

sem_static.o: sem.c
	$(CC) $(CFLAGS) -c sem.c 
shm_static.o: shm.c
	$(CC) $(CFLAGS) -c shm.c 

clean:
	$(RM) -f *.o *.a *.so