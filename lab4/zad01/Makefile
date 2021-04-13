CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= mask
# /agh/4_semestr/sysoby/lab3/zad03/

$(NAME)_static: $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -o $(NAME).out

clean:
	$(RM) -f *.o *.a *.so *.out

main: clean $(NAME)_static exec
	make run

test_exec: clean $(NAME)_static exec
	clear
	./$(NAME).out ignore
	./$(NAME).out mask
	./$(NAME).out pending

test_fork: clean $(NAME)_static exec
	clear
	./$(NAME).out ignore
	./$(NAME).out handle
	./$(NAME).out mask
	./$(NAME).out pending


exec:
	$(CC) $(CFLAGS) -$(OPT) -static exec.c -o exec.out

run:
	./$(NAME).out $(ARGS)
