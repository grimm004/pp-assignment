#include "gol.h"
#include <stdlib.h>

void error(int code) {
    switch (code) {
        case 1:
        fprintf(stderr, "Error: Invalid row length\n");
        break;
        case 2:
        fprintf(stderr, "Error: Invalid input character\n");
        break;
        case 3:
        fprintf(stderr, "Error: Could not allocate memory\n");
        break;
        default:
        break;
    }
    exit(code);
}

int mod(int a, int b) {
    const int result = a % b;
    return result < 0 ? result + b : result;
}

struct RowNode {
    char data[512];
    struct RowNode* next;
};

struct RowList {
    struct RowNode* first;
    struct RowNode* head;
};

void* allocate(int n, int size) {
    void* location = calloc(n, size);
    if (location == NULL) error(3);
    return location;
}

void read_in_file(FILE* infile, struct universe* u) {
    // Determine universe dimensions

    u->aliveCount = 0;
    u->generationCount = 0;

    u->width = 0;
    u->height = 0;

    // Load universe data

    struct RowList rowList;
    rowList.first = rowList.head = allocate(1, sizeof(struct RowNode));
    char firstPass = 1;

    char currentChar;
    int x = 0, y = 0;
    while ((currentChar = fgetc(infile)) != EOF)
        if (currentChar == '\n') {
            if (firstPass) {
                firstPass = 0;
                u->width = x;
            }
            if (x != u->width) error(1);

            struct RowNode* newHead = allocate(1, sizeof(struct RowNode));
            rowList.head->next = newHead;
            rowList.head = newHead;
            rowList.head->next = NULL;

            x = 0;
            u->height++;
            y++;
        } else if (currentChar == '*' || currentChar == '.')
            rowList.head->data[x++] = currentChar == '*';
        else error(2);
    
    u->data = (char*)allocate(u->width * u->height, sizeof(char));
    struct RowNode* currentRow = rowList.first;
    for (int y = 0; y < u->height; y++) {
        for (int x = 0; x < u->width; x++)
            u->data[(y * u->width) + x] = currentRow->data[x];
        struct RowNode* row = currentRow;
        currentRow = currentRow->next;
        free(row);
    }
}

void write_out_file(FILE *outfile, struct universe *u) {
    for (int y = 0; y < u->height; y++) {
        for (int x = 0; x < u->width; x++)
            fprintf(outfile, "%c", u->data[(y * u->width) + x] ? '*' : '.');
        fprintf(outfile, "\n");
    }
}

int is_alive(struct universe* u, int column, int row) {
    return u->data[(row * u->width) + column] & 0x01;
}

int will_be_alive(struct universe* u, int column, int row) {
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == column && y == row) && (-1 < x && x < u->width) && (-1 < y && y < u->height))
                neighbourCount += is_alive(u, x, y);

    return neighbourCount == 3 || (neighbourCount == 2 && is_alive(u, column, row));
}

int will_be_alive_torus(struct universe* u,  int column, int row) {
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == column && y == row))
                neighbourCount += is_alive(u, mod(x, u->width), mod(y, u->height));
    
    return neighbourCount == 3 || (neighbourCount == 2 && is_alive(u, column, row));
}

void evolve(struct universe* u, int (*rule)(struct universe *u, int column, int row)) {
    write_out_file(stdout, u);
    system("@cls||clear");

    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            u->aliveCount += (u->data[(y * u->width) + x] |= rule(u, x, y) << 1) >> 1;
    
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            u->data[(y * u->width) + x] = u->data[(y * u->width) + x] >> 1;
    
    u->generationCount++;
}

void print_statistics(struct universe* u) {
    int aliveCount = 0;
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            aliveCount += u->data[(y * u->width) + x];

    float alivePercentage = 100.0 * (float)aliveCount / (float)(u->width * u->height),
          averageAlivePercentage = u->generationCount ? 100.0 * (float)u->aliveCount / (float)(u->width * u->height * u->generationCount) : alivePercentage;
    
    printf("%.3f%% of cells currently alive\n%.3f%% of cells alive on average\n", alivePercentage, averageAlivePercentage);
}