CC = gcc

all: gameoflife.o libgol.so
	$(CC) gameoflife.o -Wall -Wextra -pedantic -std=c11 -lgol -L. -o gameoflife

gameoflife.o: gameoflife.c
	$(CC) gameoflife.c -Wall -Wextra -pedantic -std=c11 -c

libgol.so: gol.o
	$(CC) gol.o -Wall -Wextra -pedantic -std=c11 -shared -o libgol.so

gol.o: gol.c gol.h
	$(CC) gol.c -Wall -Wextra -pedantic -std=c11 -fPIC -c

clean:
	rm -rf gameoflife libgol.so gameoflife.o gol.o

