// Your helper functions need to be here.
#include "helpers.h"

char* customized_shell_prompt(int* offset)
{
    char* hostname = malloc(65);
    char* username = get_username();
    gethostname(hostname, 65);
    *offset = strlen(username) + strlen(hostname) + 32;
    char* result = malloc(strlen(username) + strlen(hostname) + _POSIX_PATH_MAX + 39);
    strcpy(result, KYEL "<53shell>" KGRN);
    strcpy(result+23, username);
    strcpy(result+23+strlen(username), "@");
    strcpy(result+23+strlen(username)+1, hostname);
    strcpy(result+23+strlen(username)+1+strlen(hostname), ":" KBLU);
    free(hostname);
    free(username);
    return result;
}

char* set_customized_shell(char* prompt, int offset)
{
    char* dirOffset = prompt + offset;
    getcwd(dirOffset, _POSIX_PATH_MAX);
    char* finalOffset = dirOffset + strlen(dirOffset);
    strcpy(finalOffset, KNRM "$ ");
    return prompt;
}

char* get_username()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    char* uname = malloc(strlen(pw->pw_name)+1);
    uname = pw->pw_name;
    // free(pw);
    return uname;
  }
  return "";
}

void command_print_ascii_art()
{
    printf(ICS53_ASCII_ART);
}

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

void reapTerminatedChild(List_t* bgjobs, int* exit_status)
{
    if (bgjobs == NULL) { return; }
    node_t* ptr = bgjobs->head;
    int index = 0;
    while (ptr != NULL)
    {
        bgentry_t* val = (bgentry_t*) (ptr->value);
        pid_t child_pid = val->pid;
        // printf("Checking %d\n", child_pid);
        int stopped = waitpid(val->pid, exit_status, WNOHANG);
        if (stopped)
        {
            // if child is the last process in the pipeline (or no pipe at all)
            // print the BG_TERM message and remove it from the linked list
            // otherwise, do something else?
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
    return 0;
}

// void piping(job_info* job)
// {
//     int totalChild = job->nproc;
//     // printf("TOTAL CHILD %d \n", totalChild);
//     // allocate 2d array for pipes
//     int (*pipes)[2];
//     pipes = malloc(job->nproc * sizeof(*pipes));

//     for (int i = 0; i < job->nproc; i++)
//     {
//         pipe(pipes[i]);
//     }
//     pid_t* child_pids = malloc(job->nproc * sizeof(pid_t));
//     proc_info* proc = job->procs;
//     for (int i = 0; i < job->nproc; i++)
//     {
//         child_pids[i] = fork();
//         // printf("in child %d\n", child_pids[i]);
//         if (child_pids[i] < 0)
//         {
//             perror("fork error");
//             exit(EXIT_FAILURE);
//         }
//         if (child_pids[i] == 0)
//         {
//             printf("proc details %s %s\n", proc->cmd, proc->argv[1]);
//             // child
//             close(pipes[i][1]);
//             // sleep(1);
//             printf("------\n\n");
//             printf("proc %s, i = %d\n", proc->cmd, i);
//             printf("pipes[%d]: %d, %d\n", i, pipes[i][0], pipes[i][1]);
//             if (i > 0) 
//             {
//                 dup2(pipes[i][0], STDIN_FILENO); // redirect pipe to stdin
//                 printf("redirected stdin to file %d\n", pipes[i][0]);
//             }
//             if ((i+1) < (totalChild))
//             {
//                 printf("redirected stdout to file %d\n", pipes[i+1][1]);
//                 dup2(pipes[i+1][1], STDOUT_FILENO); // redirect stdout to next pipe
//             }
//             int exec_result = execvp(proc->cmd, proc->argv);
//             // printf("should not see this\n");
//             if (exec_result < 0)
//             {
//                 printf(EXEC_ERR, proc->cmd);
//                 free_job(job);  
//                 // extern char* line;
//                 // free(line);
//                 validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated
//                 exit(EXIT_FAILURE);
//             }
//         }
//         // else
//         // {
//         //     // parent
//         // }
//         // printf("parent at i = %d\n", i);
//         proc = proc->next_proc;
//     }
//     int exit_status;
//     // wait for all child to finish
// 	sigset_t mask_child, prev;
// 	sigemptyset(&mask_child);
// 	sigaddset(&mask_child, SIGCHLD);
//     // block sigchld 
//     // sigprocmask(SIG_BLOCK, &mask_child, &prev);
//     // sleep(1);
//     // for (int i = 0; i < job->nproc; i++)
//     // {
//         int i = totalChild-1;
//         printf("waiting for child %d %d\n", i, child_pids[i]);
//         pid_t wait_result = waitpid(child_pids[i], &exit_status, 0);
//         printf("child %d dead\n", wait_result);
//     //     if (wait_result < 0) {
//     //         printf(WAIT_ERR);
//     //         exit(EXIT_FAILURE);
//     //     }
//     // }
//     // sigprocmask(SIG_SETMASK, &prev, NULL);


//     // for GS

//     for (int i = 0; i < totalChild; i++)
//     {
//         kill(child_pids[i], SIGINT);
//     }
    

//     // end 

//     free(child_pids);
//     free(pipes);
//     free_job(job);  // if a foreground job, we no longer need the data
// }

void piping(job_info* job, List_t* bgjobs)
{
    int totalChild = job->nproc;
    int totalPipes = totalChild - 1;
    // printf("TOTAL CHILD %d \n", totalChild);
    // allocate 2d array for pipes
    int (*pipes)[2];
    pipes = malloc((totalChild-1) * sizeof(*pipes));
    for (int i = 0; i < totalChild-1; i++)
    {
        pipe(pipes[i]);
    }

    pid_t* child_pids = malloc(job->nproc * sizeof(pid_t));
    proc_info* proc = job->procs;
    for (int i = 0; i < job->nproc; i++)
    {
        child_pids[i] = fork();
        // printf("in child %d\n", child_pids[i]);
        if (child_pids[i] < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        if (child_pids[i] == 0)
        {
            // in child
            // printf("proc %s, i = %d\n", proc->cmd, i);
            if (i < totalPipes)
            {
                // printf("pipes[%d]: %d, %d\n", i, pipes[i][0], pipes[i][1]);
            }
            // close all pipes not used by child
            for (int j = 0; j < totalPipes; j++)
            {
                if (i == totalPipes || j != i)
                {
                    // close not used write pipe
                    close(pipes[j][1]);
                    // printf("child %d: closed pipe %d write end, file %d\n", i, j, pipes[j][1]);
                }
                if (i == 0 || j != i-1)
                {
                    // close not used read pipe
                    close(pipes[j][0]);
                    // printf("child %d: closed pipe %d read end, file %d\n", i, j, pipes[j][0]);
                }
            }

            if (i > 0) 
            {
                dup2(pipes[i-1][0], STDIN_FILENO); // redirect pipe to stdin
                // fprintf(stderr, "child %d, redirected stdin to file %d\n",i, pipes[i-1][0]);
            }
            // if ((i+1) < (totalChild))
            if (i < totalPipes)
            {
                // fprintf(stderr, "child %d, redirected stdout to file %d\n", i, pipes[i][1]);
                dup2(pipes[i][1], STDOUT_FILENO); // redirect stdout to next pipe
            }
            int exec_result = execvp(proc->cmd, proc->argv);
            // printf("should not see this\n");
            if (exec_result < 0)
            {
                printf(EXEC_ERR, proc->cmd);
                free_job(job);  
                // extern char* line;
                // free(line);
                validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated
                exit(EXIT_FAILURE);
            }
        }
        // else
        // {
        //     // parent
        // }
        // printf("parent at i = %d\n", i);
        proc = proc->next_proc;
        // sleep(2);
    }
    // parent close all pipes
    for (int i = 0; i < totalPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // if background process, add it to list of background jobs
    if (job->bg)
    {
        bgentry_t* bgjob = malloc(sizeof(bgentry_t));
        bgjob->job = job;
        // TODO: which pid should it use? we need pid of last process for BG_TERM message
        bgjob->pid = child_pids[totalChild-1];
        time(&(bgjob->seconds));
        insertInOrder(bgjobs, bgjob);
    }
    else // if foreground process, wait for all child to finish
    {
        int exit_status;
        sigset_t mask_child, prev;
        sigemptyset(&mask_child);
        sigaddset(&mask_child, SIGCHLD);
        // block sigchld 
        sigprocmask(SIG_BLOCK, &mask_child, &prev);
        for (int i = 0; i < totalChild; i++)
        {
            // int i = totalChild-1;
            // printf("waiting for child %d %d\n", i, child_pids[i]);
            pid_t wait_result = waitpid(child_pids[i], &exit_status, 0);
            // printf("child %d dead\n", wait_result);
            if (wait_result < 0) {
                printf(WAIT_ERR);
                exit(EXIT_FAILURE);
            }
        }
        sigprocmask(SIG_SETMASK, &prev, NULL);
        free_job(job);  // if a foreground job, we no longer need the data
    }
    free(child_pids);
    free(pipes);
}
