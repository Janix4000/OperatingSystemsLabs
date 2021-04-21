CC = gcc
CFLAGS = -Wall -g
OPT = O0
NAME = main
ARGS ?=

main: clean $(NAME).c chain
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c chain.o -o $(NAME).out
	make run

chain: chain.h
	$(CC) $(CFLAGS) -$(OPT) -c -I/. chain.h -o chain.o


clean:
	$(RM) -f *.o *.a *.so *.out

run:
	./$(NAME).out $(ARGS)
