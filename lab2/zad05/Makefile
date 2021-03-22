CC = gcc
CFLAGS = -Wall -g
OPT = O0
NAME = main
ARGS ?= dane.txt kek.txt

all: main run

main: clean $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../lib -L../lib -lunifile -o $(NAME)

clean:
	$(RM) -f *.o *.a *.so

run:
	./$(NAME) $(ARGS)
