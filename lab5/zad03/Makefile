CC     := gcc
CFLAGS := -g -Wall -Wextra -ggdb3
SHELL  := /bin/bash
PIPE_NAME := consumer_producer.pipe

.PHONY: all test clean

all: consumer producer

consumer: consumer.c
	$(CC) $(CFLAGS) consumer.c -o consumer.out

producer: producer.c
	$(CC) $(CFLAGS) producer.c -o producer.out

test_files:
	mkdir -p test_files

test_files/1.txt: test_files
	yes '1' | head -c 100 | tr -d '\n' > test_files/1.txt

test_files/2.txt: test_files
	yes '2' | head -c 100 | tr -d '\n' > test_files/2.txt

test: consumer producer test_files/1.txt test_files/2.txt
	mkfifo $(PIPE_NAME)
	./producer.out $(PIPE_NAME) 2 test_files/1.txt 5 &
	./producer.out $(PIPE_NAME) 3 test_files/2.txt 5 &
	./consumer.out $(PIPE_NAME) consumer.txt 5
	rm $(PIPE_NAME)

clean:
	$(RM) consumer producer