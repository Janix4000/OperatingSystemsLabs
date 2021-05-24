CC = gcc
CFLAGS = -Wall -g 
CLIBS = -lrt -lm -lpthread
OPT = O0
ARGS ?= 16 numbers file.pgm file1.pgm
LIB = ./lib

all: clean main

run_main: main
	./main.out $(ARGS)

main: 
	$(CC) $(CFLAGS) -$(OPT) -static  main.c -o main.out $(CLIBS)

clean:
	$(RM) -f *.o *.a *.so *.out
