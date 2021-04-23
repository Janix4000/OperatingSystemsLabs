CC = gcc
CFLAGS = -Wall -g
OPT = O0
ARGS ?=

all: clean server sender

server: server.c
	$(CC) $(CFLAGS) -$(OPT) -static server.c -o server.out

sender: sender.c
	$(CC) $(CFLAGS) -$(OPT) -static sender.c -o sender.out

clean:
	$(RM) -f *.o *.a *.so *.out

runserver: server
	./server.out $(ARGS)

runsender: sender
	./sender.out $(ARGS)