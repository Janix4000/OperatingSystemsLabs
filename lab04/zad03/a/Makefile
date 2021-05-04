CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= 
# /agh/4_semestr/sysoby/lab3/zad03/

all: clean sender catcher

sender: sender.c
	$(CC) $(CFLAGS) -$(OPT) -static sender.c -o sender.out

catcher: catcher.c
	$(CC) $(CFLAGS) -$(OPT) -static catcher.c -o catcher.out

clean:
	$(RM) -f *.o *.a *.so *.out

main: clean sender catcher
	make run

run:
	./sender.out
