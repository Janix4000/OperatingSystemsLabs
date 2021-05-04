CC = gcc
CFLAGS = -Wall -g
OPT = O0
ARGS ?= 20 20
LIB = ./lib

all: clean main

run_main: main
	./main.out


run_pizzeria: pizzeria
	./pizzeria.out $(ARGS)

main: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib main.c -lipc -o main.out -lrt -lm

pizzeria: cooker delivery_man
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib pizzeria.c -lipc -o pizzeria.out -lrt -lm

cooker: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib cooker.c -lipc -o cooker.out -lrt -lm

delivery_man: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib delivery_man.c -lipc -o delivery_man.out -lrt -lm

clean:
	$(RM) -f *.o *.a *.so *.out
