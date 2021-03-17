CC = gcc
CFLAGS = -Wall -O0 -g
OPT = O0
NAME = main
ARGS = --start_measure_time --create_table 3 --merge_files ./test/small:./test/small --remove_block 1 --remove_row 0 3 --print --stop_measure_time

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../Zad1/lib_merge -L../Zad1/lib_merge -lmerge -o $(NAME)_static_$(OPT)

$(NAME)_shared_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) $(NAME).c -lmerge -o $(NAME)_shared_$(OPT)

$(NAME)_dynamic_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) $(NAME).c -ldl -DDYNAMIC -o $(NAME)_dynamic_$(OPT)

run_tests: $(NAME)_static_$(OPT) # $(NAME)_shared_$(OPT) $(NAME)_dynamic_$(OPT)
	echo "running tests for static"
	./$(NAME)_static_$(OPT) "$(cat test/test_commands)" | tee $(NAME)_static_$(OPT)_result.txt
	# echo "running tests for shared"
	# ./$(NAME)_shared_$(OPT) --quiet < test/test_commands | tee $(NAME)_shared_$(OPT)_result.txt
	# echo "running tests for dynamic"
	# ./$(NAME)_dynamic_$(OPT) --quiet < test/test_commands | tee $(NAME)_dynamic_$(OPT)_result.txt

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