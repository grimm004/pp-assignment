#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gol.h"

#define UNDEFINED -1
#define TRUE       1
#define FALSE      0

#define ERR_INVALID_ARGV      -1
#define ERR_CONFLICTING_ARGV  -2
#define ERR_INVALID_GEN_COUNT -3
#define ERR_INVALID_FILE      -4

#define ARGV_SELECTOR    0
#define ARGV_INPUT_FILE  1
#define ARGV_OUTPUT_FILE 2
#define ARGV_GEN_COUNT   3

// Output an error based on the code and exit the program with the given code
void loadError(int code) {
    switch (code) {
        case ERR_INVALID_ARGV:
        fprintf(stderr, "Error: Invalid command line argument.\n");
        break;
        case ERR_CONFLICTING_ARGV:
        fprintf(stderr, "Error: Duplicate arguments with conflicting values.\n");
        break;
        case ERR_INVALID_GEN_COUNT:
        fprintf(stderr, "Error: Invalid or conflicting generation count.\n");
        break;
        case ERR_INVALID_FILE:
        fprintf(stderr, "Error: Cloud not open universe file.\n");
        break;
        default:
        fprintf(stderr, "An unknown error occurred.\n");
        break;
    }
    exit(code);
}

int main(int argc, char* argv[]) {
    // Define default parameters
    int input = UNDEFINED, output = UNDEFINED, // input and output (when non-negative) reference the index of the filename string in the argv array
        generationCount = UNDEFINED, // undefined in order to check for duplicate arguments
        printStats = FALSE, useTorusTopology = FALSE; // defaulted to false

    // Initialise command state to be a SELECTOR
    int command = ARGV_SELECTOR;
    for (int i = 1; i < argc; i++)
        switch (command) {
            case ARGV_SELECTOR: ;
            // Access current command
            int inCommand = FALSE, charIndex = 0;
            char currentChar;
            while ((currentChar = argv[i][charIndex++]) != 0x00)
                // Invalid argument
                if (charIndex > 2) loadError(ERR_INVALID_ARGV);
                // Valid command
                else if (inCommand)
                    // Set command state to corresponding action (or set flags where applicable)
                    switch (currentChar) {
                        case 'i':
                        command = ARGV_INPUT_FILE;
                        break;
                        case 'o':
                        command = ARGV_OUTPUT_FILE;
                        break;
                        case 'g':
                        command = ARGV_GEN_COUNT;
                        break;
                        case 's':
                        printStats = TRUE;
                        break;
                        case 't':
                        useTorusTopology = TRUE;
                        break;
                        default:
                        // Unknown argument
                        loadError(ERR_INVALID_ARGV);
                        break;
                    }
                // Argument to follow
                else if (currentChar == '-') inCommand = TRUE;
                // Unknown argument
                else loadError(ERR_INVALID_ARGV);

            break;

            case ARGV_INPUT_FILE:
            // Check argument has not previously been defined with a conflicting value
            if (input != UNDEFINED && strcmp(argv[input], argv[i])) loadError(ERR_CONFLICTING_ARGV);
            input = i;
            command = ARGV_SELECTOR;
            break;

            case ARGV_OUTPUT_FILE:
            // Check argument has not previously been defined with a conflicting value
            if (output != UNDEFINED && strcmp(argv[output], argv[i])) loadError(ERR_CONFLICTING_ARGV);
            output = i;
            command = ARGV_SELECTOR;
            break;

            case ARGV_GEN_COUNT: ;
            // Check that all digits are numerical digits
            char currentGenCountChar;
            int currentGenCharIndex = 0;
            while ((currentGenCountChar = argv[i][currentGenCharIndex++]) != 0x00)
                if (!isdigit(currentGenCountChar)) loadError(ERR_INVALID_GEN_COUNT);
            
            // Convert input to integer
            int genCount;
            // Check argument has not previously been defined with a conflicting value
            if (!sscanf(argv[i], "%d", &genCount) || genCount < 0 || (generationCount != UNDEFINED && generationCount != genCount)) loadError(ERR_INVALID_GEN_COUNT);
            generationCount = genCount;
            command = ARGV_SELECTOR;
            break;

            default:
            loadError(ERR_INVALID_ARGV);
            break;
        }

    // Check commands have finished being entered
    if (command != ARGV_SELECTOR) loadError(ERR_INVALID_ARGV);
    
    // If the generation count is undefined, set it to 5
    if (generationCount == UNDEFINED) generationCount = 5;

    // Create universe on stack
    struct universe v;

    // Load the universe from a file (if defined), else load from stdin
    if (input == UNDEFINED) read_in_file(stdin, &v);
    else {
        FILE* inputFile = fopen(argv[input], "r");
        if (inputFile) {
            read_in_file(inputFile, &v);
            fclose(inputFile);
        } else loadError(ERR_INVALID_FILE);
    }

    // Loop through each generation as specified
    for (int i = 0; i < generationCount; i++)
        // Evolve the universe once (using torus topology if specified)
        evolve(&v, useTorusTopology ? will_be_alive_torus : will_be_alive);

    // Write the universe to a file (if defined), else write to stdout
    if (output == UNDEFINED) write_out_file(stdout, &v);
    else {
        FILE* outputFile = fopen(argv[output], "w");
        if (outputFile) {
            write_out_file(outputFile, &v);
            fclose(outputFile);
        } else loadError(ERR_INVALID_FILE);
    }

    // If set to print statistics, print them
    if (printStats)
        print_statistics(&v);

    return 0;
}
