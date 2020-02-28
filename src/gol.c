#include "gol.h"
#include <stdlib.h>

#define TRUE  1
#define FALSE 0

#define UNIVERSE_INTEGRITY_CONSTANT 0x12345678

#define ERR_ROW_LENGTH       1
#define ERR_INVALID_CHAR     2
#define ERR_INVALID_FILE     3
#define ERR_NO_MEMORY        4
#define ERR_NULL_UNIVERSE    5
#define ERR_INVALID_UNIVERSE 6

// Output an error based on the code and exit the program with the given code
void error(int code) {
    switch (code) {
        case ERR_ROW_LENGTH:
        fprintf(stderr, "Error: Invalid row length.\n");
        break;
        case ERR_INVALID_CHAR:
        fprintf(stderr, "Error: Invalid input character.\n");
        break;
        case ERR_INVALID_FILE:
        fprintf(stderr, "Error: Invalid universe file.\n");
        break;
        case ERR_NO_MEMORY:
        fprintf(stderr, "Error: Could not allocate memory.\n");
        break;
        case ERR_NULL_UNIVERSE:
        fprintf(stderr, "Error: Universe pointer is null.\n");
        break;
        case ERR_INVALID_UNIVERSE:
        fprintf(stderr, "Error: Universe integrity could not be validated (universe has most likely not been loaded).\n");
        break;
        default:
        fprintf(stderr, "An unknown error occurred.\n");
        break;
    }
    exit(code);
}

int integrityHash(struct universe* u) {
    return u->width ^ u->height ^ UNIVERSE_INTEGRITY_CONSTANT;
}

// Check the integrity of a universe
void validateUniverse(struct universe* u) {
    // If u is the nullptr throw an error
    if (u == NULL)
        error(ERR_NULL_UNIVERSE);
    // Else if the universe integrityHash does not match the XOR of the width, height and integrity magic number, throw an error
    else if (u->integrityHash != integrityHash(u))
        error(ERR_INVALID_UNIVERSE);
}

// A modulus function that works on negative numbers (such that 0 <= mod(a, b) < b). 
int mod(int a, int b) {
    const int result = a % b;
    return result < 0 ? result + b : result;
}

// References a single item in a (linked) RowList
struct RowNode {
    // Data with a 512 byte buffer
    char data[512];
    // Pointer to next item in list
    struct RowNode* next;
};

// References a linked list of universe rows
struct RowList {
    // First element in the list
    struct RowNode* first;
    // Head of the list
    struct RowNode* head;
};

// Util function to allocate n * size bytes
void* allocate(int n, int size) {
    void* location = calloc(n, size);
    // If no memory could be allocated, throw an error
    if (location == NULL) error(ERR_NO_MEMORY);
    return location;
}

void read_in_file(FILE* infile, struct universe* u) {
    // If file pointer is nullptr, throw error
    if (!infile) error(ERR_INVALID_FILE);
    // If universe pointer is nullptr, throw error
    if (u == NULL) error(ERR_NULL_UNIVERSE);
    // If universe is already loaded, free its data
    if (u->integrityHash == integrityHash(u)) free(u->data);

    // Initialise width and height to zero
    u->width = 0;
    u->height = 0;

    // Initialise the statistics parameters
    u->aliveCount = 0;
    u->generationCount = 1;

    // Load universe data
    struct RowList rowList;
    rowList.first = rowList.head = allocate(1, sizeof(struct RowNode));
    char firstPass = TRUE;

    char currentChar;
    // Initialise the current universe location indices to zero
    int x = 0, y = 0;
    // Read a single char at a time into currentChar (until end of file or the current row width exceeds 512 cells)
    while ((currentChar = fgetc(infile)) != EOF && x < 513)
        switch (currentChar) {
            // At each line break...
            case '\n':
            // If an initial pass has finished
            if (firstPass) {
                // Mark the initial pass as having finished
                firstPass = FALSE;
                // Set the width of the universe
                u->width = x;
            }
            // If the width of the current row is not equal to the width of the universe, throw an error
            if (x != u->width) error(ERR_ROW_LENGTH);

            // Create a new row node in the linked RowList
            struct RowNode* newHead = allocate(1, sizeof(struct RowNode));
            rowList.head->next = newHead;
            rowList.head = newHead;
            rowList.head->next = NULL;

            // REset the x location index and increment the height and y index
            x = 0;
            u->height++;
            y++;
            break;

            // At each asterisk...
            case '*':
            // Mark the current data point in the universe as alive and post-increment the x index
            rowList.head->data[x++] = TRUE;
            u->aliveCount++;
            break;

            // At each dot...
            case '.':
            // Mark the current data point in the universe as dead and post-increment the x index
            rowList.head->data[x++] = FALSE;
            break;

            // At unrecognised character...
            default:
            // Throw an error
            error(ERR_INVALID_CHAR);
            break;
        }
    
    // If there are more than 512 cells in a row, throw an error
    if (x > 512) error(ERR_ROW_LENGTH);
    // If x is non-zero (no new line at end of file), throw an error
    else if (x != 0) error(ERR_INVALID_FILE);

    // Allocate required space for the one-dimensional universe data array
    u->data = (char*)allocate(u->width * u->height, sizeof(char));
    // Store the current row being processed
    struct RowNode* currentRow = rowList.first;
    // For each row...
    for (int y = 0; y < u->height; y++) {
        // For each column...
        for (int x = 0; x < u->width; x++)
            // Set the alive state of the current (row, column) position in the universe data
            u->data[(y * u->width) + x] = currentRow->data[x];
        // Switch to the next row
        struct RowNode* row = currentRow;
        currentRow = currentRow->next;
        // Free the processed row that was just processed
        free(row);
    }

    // Set the integrity hash to the XOR of the width, height and integrity magic number
    u->integrityHash = integrityHash(u);
}

void write_out_file(FILE *outfile, struct universe *u) {
    // If file pointer is nullptr, throw error
    if (!outfile) error(ERR_INVALID_FILE);
    // Validate the integrity of the universe
    validateUniverse(u);

    for (int y = 0; y < u->height; y++) {
        for (int x = 0; x < u->width; x++)
            fprintf(outfile, "%c", is_alive(u, x, y) ? '*' : '.');
        fprintf(outfile, "\n");
    }
}

int is_alive(struct universe* u, int column, int row) {
    // Validate the integrity of the universe
    validateUniverse(u);

    // Return the corresponding cell state (masked with 0x01)
    return u->data[(row * u->width) + column] & 0x01;
}

int will_be_alive(struct universe* u, int column, int row) {
    // Validate the integrity of the universe
    validateUniverse(u);

    // Calculate the number of neighbours surrounding the cell
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == column && y == row) && (-1 < x && x < u->width) && (-1 < y && y < u->height))
                neighbourCount += is_alive(u, x, y);

    // Return the outcome cell state
    return neighbourCount == 3 || (neighbourCount == 2 && is_alive(u, column, row));
}

int will_be_alive_torus(struct universe* u,  int column, int row) {
    // Validate the integrity of the universe
    validateUniverse(u);

    // Calculate the number of neighbours surrounding the cell (using modulo to loop back for torus)
    int neighbourCount = 0;
    for (int y = row - 1; y < row + 2; y++)
        for (int x = column - 1; x < column + 2; x++)
            if (!(x == column && y == row))
                neighbourCount += is_alive(u, mod(x, u->width), mod(y, u->height));

    // Return the outcome cell state
    return neighbourCount == 3 || (neighbourCount == 2 && is_alive(u, column, row));
}

void evolve(struct universe* u, int (*rule)(struct universe *u, int column, int row)) {
    // Validate the integrity of the universe
    validateUniverse(u);

    // Loop through each cell in the universe
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            // Calculate the next state of the cell, store it in the second least significant bit ...
            // ...in the universe data array and add one to the alive count (if the cell will be alive).
            u->aliveCount += (u->data[(y * u->width) + x] |= rule(u, x, y) << 1) >> 1;

    // Loop through each cell in the universe
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            // Right-shift the cell byte to set the new state
            u->data[(y * u->width) + x] = u->data[(y * u->width) + x] >> 1;

    // Increment the total generation count
    u->generationCount++;
}

void print_statistics(struct universe* u) {
    // Validate the integrity of the universe
    validateUniverse(u);

    // Calculate the current number of alive cells in the universe
    int aliveCount = 0;
    for (int y = 0; y < u->height; y++)
        for (int x = 0; x < u->width; x++)
            aliveCount += u->data[(y * u->width) + x];

    // Calculate the current and average alive percentages
    float alivePercentage = (u->width * u->height) ? 100.0 * (float)aliveCount / (float)(u->width * u->height) : 0,
          averageAlivePercentage = (u->width * u->height) ? 100.0 * (float)u->aliveCount / (float)(u->width * u->height * u->generationCount) : 0;

    // Output the percentages in the required format
    printf("%.3f%% of cells currently alive\n%.3f%% of cells alive on average\n", alivePercentage, averageAlivePercentage);
}