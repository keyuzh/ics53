#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  if (fork() == 0) {
    /*this is the child*/
    while (1)
    {
    // printf("Terminating child : PID=%d\n", getpid());
    printf("Running child : PID=%d\n", getpid());
    }
    exit(0);
  }

  /* parent code */
  // while (1) {
    printf("Running parent : PID=%d\n", getpid());
    printf("Terminating parent : PID=%d\n", getpid());
    sleep(10);
  // }
}
