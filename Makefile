CC = gcc
CFLAGS = -Wall -pthread

all: main

main: main.c intersection.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f main
