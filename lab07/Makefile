CC = gcc
CFLAGS = -Wall -g
OPT = O0
ARGS ?=
LIB = ./lib

all: clean main

main: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib main.c -lipc -o main.out -lrt -lm

clean:
	$(RM) -f *.o *.a *.so *.out
