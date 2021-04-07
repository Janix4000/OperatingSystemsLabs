CC = gcc
CFLAGS = -Wall -g
OPT ?= O0
NAME = main
ARGS ?= --start_measure_time --create_table 3 --merge_files ./test/small:./test/small --remove_block 1 --remove_row 0 3 --print --stop_measure_time

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../../lab1/Zad1/lib_merge/ -L../../lab1/Zad1/lib_merge/ -lmerge -o $(NAME)_static_$(OPT)

run_tests: $(NAME)_static_$(OPT)
	mkdir -p result
	echo "running tests.txt for static"
	cat ./test/test_commands.txt | xargs ./$(NAME)_static_$(OPT) | tee result/$(NAME)_static_without_fork_$(OPT).txt
	make clean

clean:
	$(RM) -f *.o *.a *.so

main: clean $(NAME)_static_$(OPT)
	make run

run:
	cat ./test/test_commands.txt | xargs ./$(NAME)_static_$(OPT)

vg: 
	cat ./test/test_commands.txt | \ 
	xargs vg \
	./$(NAME)_static_$(OPT)