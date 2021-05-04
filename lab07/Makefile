CC = gcc
CFLAGS = -Wall -g
OPT = O0
ARGS ?=
LIB = ./lib

all: clean main

run_main: main
	./main.out


run_pizzeria: pizzeria
	./pizzeria.out

main: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib main.c -lipc -o main.out -lrt -lm

pizzeria: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib pizzeria.c -lipc -o pizzeria.out -lrt -lm

cooker: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib cooker.c -lipc -o cooker.out -lrt -lm

clean:
	$(RM) -f *.o *.a *.so *.out
