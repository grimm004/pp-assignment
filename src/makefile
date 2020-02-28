all: gameoflife.o gol.o gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -o gameoflife gameoflife.o gol.o

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c gol.c

gameoflife.o: gameoflife.c
	gcc -Wall -Wextra -pedantic -std=c11 -c gameoflife.c

clean:
	rm -rf gameoflife gameoflife.o gameoflife.o
