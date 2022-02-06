
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[1;31m"

int main(int argc, char* argv[]) {

    printf ("Value :: %d \n", _POSIX_PATH_MAX);
    printf ("color :: %d \n", strlen(KRED));
    printf ("normal :: %d \n", strlen(KNRM));
    for (int i = 0; i < 5; i++)
    {
        sleep(1);
        printf("%d\n", i);
    }
}
