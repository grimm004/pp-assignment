CC = gcc

all: libgol.so gameoflife.c
	$(CC) -Wall -Wextra -pedantic -std=c11 gameoflife.c -lgol -L. -o gameoflife

libgol.so: gol.o
	$(CC) -Wall -Wextra -pedantic -std=c11 -shared gol.o -o libgol.so

gol.o: gol.c gol.h
	$(CC) -Wall -Wextra -pedantic -std=c11 -fPIC -c gol.c

clean:
	rm -rf gameoflife gol.o libgol.so

