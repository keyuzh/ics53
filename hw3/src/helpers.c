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
    int index = 0;
    while (ptr != NULL)
    {
        bgentry_t* val = (bgentry_t*) (ptr->value);
        pid_t child_pid = val->pid;
        // printf("Checking %d\n", child_pid);
        int status;
        int stopped = waitpid(val->pid, &status, WNOHANG);
        if (stopped)
        {
            fprintf(stdout, BG_TERM, stopped, val->job->line);
            // remove node from linked list
            ptr = ptr->next;
            removeByIndex(bgjobs, index);
            continue;
        }
        ptr = ptr->next;
        ++index;
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

void print_rd_err()
{
    fprintf(stderr, RD_ERR);
}

int checkRediretionCombination(proc_info* proc)
{
    int res = 1;
    if (proc->in_file != NULL && proc->out_file != NULL)
    {
        res *= strcmp(proc->in_file, proc->out_file);
    }
    if (proc->in_file != NULL && proc->err_file != NULL)
    {
        res *= strcmp(proc->in_file, proc->err_file);
    }
    if (proc->in_file != NULL && proc->outerr_file != NULL)
    {
        res *= strcmp(proc->in_file, proc->outerr_file);
    }
    if (proc->out_file != NULL && proc->err_file != NULL)
    {
        res *= strcmp(proc->out_file, proc->err_file);
    }
    if (proc->out_file != NULL && proc->outerr_file != NULL)
    {
        res *= strcmp(proc->out_file, proc->outerr_file);
    }
    if (proc->err_file != NULL && proc->outerr_file != NULL)
    {
        res *= strcmp(proc->err_file, proc->outerr_file);
    }
    return res;
}

int redirection(proc_info* proc)
{
    if (checkRediretionCombination(proc) == 0)
    {
        print_rd_err();
        return -1;
    }
    if (proc->in_file != NULL)
    {
        int in_file_desc = open(proc->in_file, O_RDONLY);
        if (in_file_desc == -1)
        { 
            print_rd_err(); 
            return -1;
        }
        dup2(in_file_desc, STDIN_FILENO);
    }
    if (proc->out_file != NULL)
    {
        int out_file_desc = open(proc->out_file, O_WRONLY | O_TRUNC | O_CREAT);
        if (out_file_desc == -1)
        { 
            print_rd_err(); 
            return -1;
        }
        dup2(out_file_desc, STDOUT_FILENO);
    }
    if (proc->err_file != NULL)
    {
        int err_file_desc = open(proc->err_file, O_WRONLY | O_TRUNC | O_CREAT);
        if (err_file_desc == -1)
        { 
            print_rd_err(); 
            return -1;
        }
        dup2(err_file_desc, STDERR_FILENO);
    }
    if (proc->outerr_file != NULL)
    {
        int outerr_file_desc = open(proc->outerr_file, O_WRONLY | O_TRUNC | O_CREAT | O_APPEND);
        if (outerr_file_desc == -1)
        { 
            print_rd_err(); 
            return -1;
        }
        dup2(outerr_file_desc, STDOUT_FILENO);
        dup2(outerr_file_desc, STDERR_FILENO);
    }
    
}