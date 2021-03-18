CC = gcc
CFLAGS = -Wall -g
OPT = O2
NAME = main
ARGS = --start_measure_time --create_table 3 --merge_files ./test/small:./test/small --remove_block 1 --remove_row 0 3 --print --stop_measure_time

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../Zad1/lib_merge -L../Zad1/lib_merge -lmerge -o $(NAME)_static_$(OPT)

$(NAME)_shared_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) $(NAME).c -I../Zad1/lib_merge ../Zad1/lib_merge/libmerge.so -o $(NAME)_shared_$(OPT)

$(NAME)_dynamic_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) $(NAME).c -ldl -DDYNAMIC -I../Zad1/lib_merge -L../Zad1/lib_merge -o $(NAME)_dynamic_$(OPT)

run_tests: $(NAME)_static_$(OPT) $(NAME)_shared_$(OPT) $(NAME)_dynamic_$(OPT)
	mkdir -p result
	echo "running tests for static"
	cat ./test/test_commands | xargs ./$(NAME)_static_$(OPT)
	echo "running tests for shared"
	cat ./test/test_commands | xargs ./$(NAME)_shared_$(OPT)
	echo "running tests for dynamic"
	cat ./test/test_commands | xargs ./$(NAME)_dynamic_$(OPT)
	make clean

clean:
	$(RM) -f *.o *.a *.so

main: clean $(NAME)_static_$(OPT)
	make run

run:
	./$(NAME)_static_$(OPT) $(ARGS)

rerun:
	clear
	make -C ../Zad1/lib_merge
	make main