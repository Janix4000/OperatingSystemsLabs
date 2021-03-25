CC = gcc
CFLAGS = -Wall -g
OPT = O0
NAME = main
ARGS ?= a b

main: clean $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../lib -L../lib -lunifile -o $(NAME)
	make run
	

clean:
	$(RM) -f *.o *.a *.so

run:
	./$(NAME) $(ARGS)
