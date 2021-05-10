CC = gcc
CFLAGS = -Wall -g 
CLIBS = -lrt -lm -lpthread -lipc
OPT = O0
ARGS ?= 20 20
LIB = ./lib

all: clean main

run_main: main
	./main.out


run_pizzeria: pizzeria
	./pizzeria.out $(ARGS)

main: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib main.c -lipc -o main.out $(CLIBS)

pizzeria: cooker delivery_man
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib pizzeria.c -lipc -o pizzeria.out $(CLIBS)

cooker: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib cooker.c -lipc -o cooker.out $(CLIBS)

delivery_man: 
	$(CC) $(CFLAGS) -$(OPT) -static -L./lib delivery_man.c  -o delivery_man.out $(CLIBS)

clean:
	$(RM) -f *.o *.a *.so *.out
