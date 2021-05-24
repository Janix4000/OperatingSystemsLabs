CC     := gcc
CFLAGS := -g2 -Wall -pthread

.PHONY: all clean run

run: clean all
	./main2


all: main2
	
main: main2.c
	$(CC) $(CFLAGS) main2.c -o main2.out 

clean:
	$(RM) main2.out