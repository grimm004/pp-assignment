#include <stdio.h>
#include "gol.h"

int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++)
        printf("%s\n", argv[i]);
    
    struct universe v;
    read_in_file(fopen("glider.txt", "r"), &v);

    write_out_file(stdout, &v);

    printf("%d", -10 % 10);

    // evolve(&v,will_be_alive);
    // evolve(&v,will_be_alive);
    // evolve(&v,will_be_alive);
    // evolve(&v,will_be_alive);
    // evolve(&v,will_be_alive);

    // write_out_file(stdout,&v);

    return 0;
}
