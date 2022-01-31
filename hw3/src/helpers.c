// Your helper functions need to be here.
#include "helpers.h"

void command_cd(const char* path) 
{
    if (chdir(path) != 0)
    {
        // error
        fprintf(stderr, DIR_ERR);
    }
    else 
    {
        char* dir = getcwd(NULL, 0);
        fprintf(stdout, "%s\n", dir);
        free(dir);
    }
}

void sigusr2_handler(int sig) 
{
    pid_t pid = getpid();
    fprintf(stdout, "Hi User! I am process %d\n", pid);
}
