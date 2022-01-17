#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
    char *response = NULL;
    size_t len;
    while(getline(&response, &len, stdin) > 0){
        printf("%s", response);
        free(response);
    }
    return 0;
}
