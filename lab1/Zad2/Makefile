CC = gcc
CFLAGS = -Wall -O0 -g
OPT = O0
NAME = main
ARGS = --start_measure_time --create_table 3 --merge_files ./test/small:./test/small --remove_block 1 --remove_row 0 3 --print --stop_measure_time

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../Zad1/lib_merge -L../Zad1/lib_merge -lmerge -o $(NAME)_static_$(OPT)

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so

main: clean $(NAME)_static_$(OPT)
	make run

run:
	./$(NAME)_static_$(OPT) $(ARGS)

rerun:
	clear
	make -C ../Zad1/lib_merge
	make main