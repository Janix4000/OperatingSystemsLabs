CC = gcc
CFLAGS = -Wall -g
OPT = O0
ARGS ?=
LIB = ./lib

all: clean server sender

ips: $(LIB)/ips.c
	$(CC) $(CFLAGS) -$(OPT) -c $(LIB)/ips.c

server: ips server.c
	$(CC) $(CFLAGS) -$(OPT) -static server.c ips.o -o server.out

sender: ips sender.c
	$(CC) $(CFLAGS) -$(OPT) -static sender.c ips.o -o sender.out

clean:
	$(RM) -f *.o *.a *.so *.out

runserver: server
	./server.out $(ARGS)

runsender: sender
	./sender.out $(ARGS)