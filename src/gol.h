#include <stdio.h>

struct universe {
    // A hash to determine if a universe is loaded (as the library functions do not account or make assumptions for this)
    int integrityHash;

    // A one-dimensional array storing the universe data
    char* data;
    // The width and height of the universe
    int width, height;
    // The total number of generations and alive cells (from all generations)
    int generationCount, aliveCount;
};

/*Do not modify the next seven lines*/
void read_in_file(FILE *infile, struct universe *u);
void write_out_file(FILE *outfile, struct universe *u);
int is_alive(struct universe *u, int column, int row);
int will_be_alive(struct universe *u, int column, int row);
int will_be_alive_torus(struct universe *u,  int column, int row);
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row));
void print_statistics(struct universe *u);
/*You can modify after this line again*/
