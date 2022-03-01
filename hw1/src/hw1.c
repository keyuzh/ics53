#include "hw1.h"
#include <stdio.h>
#include <string.h>
// You may define any helper functions you want. Place them in helpers.c/.h

// Main program
int main(int argc, char *argv[]) {
    Options opt;
    checkOptions(&opt, argc, argv);

    char* message;
    Stats res;
    setbuf(stdout, NULL);
    switch (opt.mode)
    {
    case 'L':
        message = modeLFormat(argv[2], opt.s);
        res = modeLprocess(message, opt);
        break;
    case 'W':
        message = modeWFormat(argv[2]);
        res = modeWprocess(message, opt);
        break;
    }
    free(message);
    fclose(stdin);
    fclose(stdout);
    fprintf(stderr, "%u\n", res.match);
    exit(res.exitcode);
    return 0;
}
