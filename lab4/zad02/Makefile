CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= 
# /agh/4_semestr/sysoby/lab3/zad03/

$(NAME)_static: $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -o $(NAME).out

clean:
	$(RM) -f *.o *.a *.so *.out

main: clean $(NAME)_static exec
	make run

run:
	./$(NAME).out $(ARGS)
