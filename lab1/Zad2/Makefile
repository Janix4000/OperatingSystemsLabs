CC = gcc
CFLAGS = -Wall -O0

# static:
# 	$(CC) $(CFLAGS) -c lib_vector.c -o lib_vector.o 
# 	$(CC) $(CFLAGS) -c lib_merge.c -o lib_merge.o 
# 	ar rcs lib_merge.a lib_merge.o lib_vector.o

clean:
	rm -f *.o 
	rm -f *.a 
	rm -f *.so
	rm -f *merged*

main1: clean #static
	$(CC) $(CFLAGS) -c main.c -o main.o
	#$(CC) $(CFLAGS) -static main.o -L. lib_merge.a -o main
	$(CC) $(CFLAGS) -static main.o -o main
	make run

run:
	clear
	./main --create_table 3 --merge_files ./kek:./lol ./moar:./reakt --remove_block 2 --remove_row 2 3 --start_measure_time
