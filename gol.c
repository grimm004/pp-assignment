#include "gol.h"
#include "stdio.h"
#include <stdlib.h>

int mod(int a, int b) {
    const int result = a % b;
    return result < 0 ? result + b : result;
}

void read_in_file(FILE* infile, struct universe* u) {
    // Determine universe dimensions

    u->aliveCount = 0;
    u->generationCount = 0;

    u->width = 0;
    while (fgetc(infile) != '\n') {
        u->width++;
    }

    fseek(infile, 0L, SEEK_END);
    u->height = (ftell(infile) / (u->width + 1));

    rewind(infile);

    printf("Columns: %d, Rows: %d\n", u->width, u->height);

    // Load universe data

    u->data = (char*)malloc(u->width * u->height);

    char currentChar;
    int x = 0, y = 0;
    while ((currentChar = fgetc(infile)) != EOF) {
        if (currentChar == '\n') {
            if (x != u->width) {
                fprintf(stderr, "Error: Invalid row length (%d, expected %d).", x, u->width);
                exit(1);
            }
            x = 0;
            y++;
            continue;
        } else if (currentChar == '*' || currentChar == '.')
            u->data[(y * u->width) + x++] = currentChar == '*';
        else {
            fprintf(stderr, "Error: Invalid character ('%c', expected '*' or '-').", (char)currentChar);
            exit(1);
        }
    }

    fclose(infile);
}

void write_out_file(FILE *outfile, struct universe *u) {
    for (int y = 0; y < u->height; y++) {
        for (int x = 0; x < u->width; x++)
            fprintf(outfile, "%c", u->data[(y * u->width) + x] ? '*' : '.');
        fprintf(outfile, "\n");
    }
}

int is_alive(struct universe* u, int column, int row) {
    return u->data[(row * u->width) + column];
}

int will_be_alive(struct universe* u, int column, int row) {
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == row && y == column) && (-1 < x && x < u->width) && (-1 < y && y < u->height))
                neighbourCount += is_alive(u, x, y);
    
    return neighbourCount == 3 || (is_alive(u, column, row) && neighbourCount == 2);
}

int will_be_alive_torus(struct universe* u,  int column, int row) {
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == row && y == column))
                neighbourCount += is_alive(u, mod(x, u->width), mod(y, u->height));
    
    return neighbourCount == 3 || (is_alive(u, column, row) && neighbourCount == 2);
}

void evolve(struct universe* u, int (*rule)(struct universe *u, int column, int row)) {
    char* nextGeneration = (char*)malloc(u->width * u->height);
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            u->aliveCount += nextGeneration[(y * u->width) + x] = rule(u, x, y);
    
    u->generationCount++;

    free(u->data);
    u->data = nextGeneration;
}

void print_statistics(struct universe* u) {
    int aliveCount = 0;
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            aliveCount += u->data[(y * u->width) + x];

    float alivePercentage = 100.0 * (float)aliveCount / (float)(u->width * u->height),
          averageAlivePercentage = 100.0 * (float)u->aliveCount / (float)(u->width * u->height * u->generationCount);
    
    printf("%.3f%% of cells currently alive\n%.3f%% of cells alive on average\n", alivePercentage, averageAlivePercentage);
}
