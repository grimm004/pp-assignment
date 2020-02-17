#include <stdio.h>
#include "gol.h"
#include <string.h>

int main(int argc, char **argv) {
    int command = 0;

    int input = -1, output = -1,
        generationCount = 5,
        printStats = 0,useTorusTopology = 0;

    for (int i = 0; i < argc; i++) {
        switch (command) {
            case 0:
            if (strcmp(argv[i], "-i") == 0) command = 1;
            else if (strcmp(argv[i], "-o") == 0) command = 2;
            else if (strcmp(argv[i], "-g") == 0) command = 3;
            else if (strcmp(argv[i], "-s") == 0) printStats = 1;
            else if (strcmp(argv[i], "-t") == 0) useTorusTopology = 1;
            break;

            case 1:
            input = i;
            command = 0;
            break;

            case 2:
            output = i;
            command = 0;
            break;

            case 3:
            sscanf(argv[i], "%d", &generationCount);
            command = 0;
            break;

            default:
            command = 0;
        }
    }

    struct universe v;
    read_in_file(input == -1 ? stdin : fopen(argv[input], "r"), &v);

    while (1)//for (int i = 0; i < generationCount; i++)
        evolve(&v, useTorusTopology ? will_be_alive_torus : will_be_alive);

    write_out_file(output == -1 ? stdout : fopen(argv[output], "w"), &v);

    if (printStats)
        print_statistics(&v);

    return 0;
}
