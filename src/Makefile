CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

all: libgol.so gameoflife.c
	$(CC) gameoflife.c -lgol -L. -o gameoflife

libgol.so: gol.o
	$(CC) -shared gol.o -o libgol.so

gol.o: gol.c gol.h
	$(CC) -fPIC -c gol.c

clean:
	rm -rf gameoflife gol.o libgol.so

