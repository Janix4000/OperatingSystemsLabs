CC = gcc
CFLAGS = -Wall -O0
OPT = O0
NAME = main
# static:
# 	$(CC) $(CFLAGS) -c lib_vector.c -o lib_vector.o 
# 	$(CC) $(CFLAGS) -c lib_merge.c -o lib_merge.o 
# 	ar rcs lib_merge.a lib_merge.o lib_vector.o

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so
	rm -f *merged*

$(NAME)_static_$(OPT): $(NAME).c
	$(CC) $(CFLAGS) -$(OPT) -static $(NAME).c -I../Zad1/lib_merge -L../Zad1/lib_merge -lmerge -o $(NAME)_static_$(OPT)

main1: clean $(NAME)_static_$(OPT)
	
	# $(CC) $(CFLAGS) -c main.c -o main.o
	# #$(CC) $(CFLAGS) -static main.o -L. lib_merge.a -o main
	# $(CC) $(CFLAGS) -static main.o -o main
	make run

run:
	clear
	./main --create_table 3 --merge_files ./kek:./lol ./moar:./reakt --remove_block 2 --remove_row 2 3 --start_measure_time
