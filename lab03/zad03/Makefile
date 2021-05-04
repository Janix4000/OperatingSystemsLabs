CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= ./testFolder a 2
# /agh/4_semestr/sysoby/lab3/zad03/

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../../lab1/Zad1/lib_merge/ -L../../lab1/Zad1/lib_merge/ -lmerge -o $(NAME)_static_$(OPT)

run_tests: $(NAME)_static_$(OPT)
	mkdir -p result
	echo "running tests for static"
	cat ./test/test_commands | xargs ./$(NAME)_static_$(OPT)
	make clean

clean:
	$(RM) -f *.o *.a *.so

main: clean $(NAME)_static_$(OPT)
	make run

run:
	./$(NAME)_static_$(OPT) $(ARGS)
