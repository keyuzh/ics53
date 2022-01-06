#include "hw1.h"
#include <stdio.h>
#include <string.h>
// You may define any helper functions you want. Place them in helpers.c/.h

// Main program
int main(int argc, char *argv[]) {

    char mode = getMode(argv);
    // printf("Mode: %c\n", mode);
    int modeS = isModeS(argc, argv);

    char* message;
    unsigned int numHidden;
    switch (mode)
    {
    case 'L':
        message = modeLFormat(argv[2], modeS);
        numHidden = modeLprocess(message, modeS);
        break;
    
    default:
        break;
    }

    fprintf(stderr, "%u\n", numHidden);

    free(message);

    return 0;
}
