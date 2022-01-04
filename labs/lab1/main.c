// Keyu Zhang
// keyuz4
#include <stdio.h>
#include <string.h>

int main() {
    char* netid = "keyuz4";
    int fiftythree = 53;

    printf("Hello, %s! Welcome to ICS%d!\n", netid, fiftythree);

    for (int i = strlen(netid); i >= 0; --i)
    {
        printf("%d\n", i);
    }
    

    return 0;
}
