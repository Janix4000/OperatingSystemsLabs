CC = gcc
CFLAGS = -Wall -g 
CLIBS = -lrt -lm -lpthread
OPT = O0
ARGS ?= 20 20
LIB = ./lib

all: clean main

run_main: main
	./main.out

main: 
	$(CC) $(CFLAGS) -$(OPT) -static  main.c -o main.out $(CLIBS)

clean:
	$(RM) -f *.o *.a *.so *.out
