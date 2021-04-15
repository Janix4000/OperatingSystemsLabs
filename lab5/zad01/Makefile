CC = gcc
CFLAGS = -Wall -g
OPT = O0
NAME = main
ARGS ?=

main: clean $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -o $(NAME).out
	make run

clean:
	$(RM) -f *.o *.a *.so *.out

run:
	./$(NAME).out $(ARGS)
