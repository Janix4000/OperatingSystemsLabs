CC     := gcc
CFLAGS := -g2 -Wall

.PHONY: all clean run

run: clean all
	./main2.out


all: main2
	
main: main2.c
	$(CC) $(CFLAGS) main2.c -o main2.out -pthread

clean:
	$(RM) main2.out