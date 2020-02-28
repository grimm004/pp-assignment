all: libgol.so gameoflife.c
	gcc -Wall -Wextra -pedantic -std=c11 -o gameoflife gameoflife.c -lgol -L.

libgol.so: gol.o
	gcc -Wall -Wextra -pedantic -std=c11 -shared gol.o -o libgol.so

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c gol.c

clean:
	rm -rf gameoflife gol.o libgol.so

