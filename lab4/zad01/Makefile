CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= handle
# /agh/4_semestr/sysoby/lab3/zad03/

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -o $(NAME)_static_$(OPT).out

clean:
	$(RM) -f *.o *.a *.so *.out

main: clean $(NAME)_static_$(OPT)
	make run

run:
	./$(NAME)_static_$(OPT).out $(ARGS)
