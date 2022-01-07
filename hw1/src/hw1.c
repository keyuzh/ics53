#include "hw1.h"
#include <stdio.h>
#include <string.h>
// You may define any helper functions you want. Place them in helpers.c/.h

// Main program
int main(int argc, char *argv[]) {

    Options opt;
    checkOptions(&opt, argc, argv);

    // opt.mode = getMode(argv);
    
    // opt.s = isModeS(argc, argv);
    // opt.o = isModeOther(argc, argv, 'O');
    // opt.p = isModeOther(argc, argv, 'P');

    
    char* message;
    Wstats res;
    switch (opt.mode)
    {
    case 'L':
        message = modeLFormat(argv[2], opt.s);
        res = modeLprocess(message, opt);
        break;

    case 'W':
        message = modeWFormat(argv[2]);
        res = modeWprocess(message, opt);
    
    default:
        break;
    }

    fprintf(stderr, "%u\n", res.match);

    free(message);

    exit(res.exitcode);
    return 0;
}
