CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= 
# /agh/4_semestr/sysoby/lab3/zad03/

all: nodefer resethand info

nodefer: clean nodefer.c
	$(CC) $(CFLAGS) -$(OPT) -static nodefer.c -o nodefer.out
	./nodefer.out

resethand: clean resethand.c
	$(CC) $(CFLAGS) -$(OPT) -static resethand.c -o resethand.out
	./resethand.out

info: clean info.c
	$(CC) $(CFLAGS) -$(OPT) -static info.c -o info.out
	./info.out

clean:
	$(RM) -f *.o *.a *.so *.out

main: clean $(NAME)_static
	make run

run:
	./$(NAME).out $(ARGS)
