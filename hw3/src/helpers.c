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

void sigchld_handler(int sig)
{
    // waitpid()
    // fprintf(stdout, "------------SIGCHLD %d -------------\n", getpid());
    extern int CHECK_BACKGROUND_JOBS;
	CHECK_BACKGROUND_JOBS = 1;
}

int bgentry_tComparer(void* entry1, void* entry2)
{
    bgentry_t* e1 = (bgentry_t*) entry1;
    bgentry_t* e2 = (bgentry_t*) entry2;
    return e1->seconds - e2->seconds;
}

void reapTerminatedChild(List_t* bgjobs)
{
    if (bgjobs == NULL) { return; }
    node_t* ptr = bgjobs->head;
    for (int i = 0; i < bgjobs->length; i++)
    {
        bgentry_t* val = (bgentry_t*) (ptr->value);
        pid_t child_pid = val->pid;
        // printf("Checking %d\n", child_pid);
        int status;
        int stopped = waitpid(val->pid, &status, WNOHANG);
        if (stopped)
        {
            fprintf(stdout, BG_TERM, stopped, val->job->line);
        }
        ptr = ptr->next;
    }
    // #define CHECK_BACKGROUND_JOBS 0
}

void command_bglist(List_t* bgjobs)
{
    if (bgjobs == NULL) { return; }
    node_t* ptr = bgjobs->head;
    for (int i = 0; i < bgjobs->length; i++)
    {
        print_bgentry((bgentry_t*)(ptr->value));
        ptr = ptr->next;
    }
}

void killAllChild(List_t* bgjobs)
{
    if (bgjobs == NULL) { return; }
    node_t* ptr = bgjobs->head;
    for (int i = 0; i < bgjobs->length; i++)
    {
        bgentry_t* val = (bgentry_t*) (ptr->value);
        kill(val->pid, SIGKILL); // which signal to send? 
        fprintf(stdout, BG_TERM, val->pid, val->job->line);
        ptr = ptr->next;
    }
}