#include "hw1.h"
#include <stdio.h>
#include <string.h>
// You may define any helper functions you want. Place them in helpers.c/.h

// Main program
int main(int argc, char *argv[]) {

    Options opt;

    opt.mode = getMode(argv);
    // printf("Mode: %c\n", mode);
    opt.s = isModeS(argc, argv);
    opt.o = isModeOther(argc, argv, 'O');
    opt.p = isModeOther(argc, argv, 'P');

    char* message;
    unsigned int numHidden;
    switch (opt.mode)
    {
    case 'L':
        message = modeLFormat(argv[2], opt.s);
        numHidden = modeLprocess(message, opt);
        break;
    
    default:
        break;
    }

    fprintf(stderr, "%u\n", numHidden);

    free(message);

    return 0;
}
