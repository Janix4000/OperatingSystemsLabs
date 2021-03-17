CC = gcc
CFLAGS = -Wall -O0
OPT = O0
NAME = main

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../Zad1/lib_merge -L../Zad1/lib_merge -lmerge -o $(NAME)_static_$(OPT)

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so

main: clean $(NAME)_static_$(OPT)
	make run

run:
	./$(NAME)_static_$(OPT) --create_table 3 --merge_files ./kek:./lol ./moar:./reakt --remove_block 2 --remove_row 2 3 --start_measure_time --stop_measure_time

rerun:
	clear
	make -C ../Zad1/lib_merge
	make main