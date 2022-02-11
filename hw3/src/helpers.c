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
    if (prompt == NULL)
    {
        return "";
    }
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
  char* uname;
  if (pw)
  {
    uname = malloc(strlen(pw->pw_name)+1);
    strcpy(uname, pw->pw_name);
    return uname;
  }
  uname = malloc(2);
  strcpy(uname, "");
  return uname;
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
    pid_t child_pid;
    while ((child_pid = waitpid(-1, exit_status, WNOHANG)) > 0)
    {
        int index = 0;
        node_t* ptr = bgjobs->head;
        // if pid in bglist, print BG_TERM message and remove if from list
        while (ptr != NULL)
        {
            bgentry_t* val = (bgentry_t*) (ptr->value);
            if (child_pid == val->pid)
            {
                // remove node from linked list
                bgentry_t* toDelete = (bgentry_t*) removeByIndex(bgjobs, index);
                fprintf(stdout, BG_TERM, child_pid, val->job->line);
                free_job(toDelete->job);
                free(toDelete);
                break;
            }
            ptr = ptr->next;
            ++index;
        }
    }
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

void command_fg(List_t* bgjobs, int* exit_status, pid_t pid)
{
    if ((bgjobs == NULL) || (bgjobs->length == 0))
    {
        printf(PID_ERR);
        return;
    }

    bgentry_t* recent = NULL;
    if (pid == -1)
    {
        recent = (bgentry_t*) removeRear(bgjobs);  // most recent
    }
    else
    {
        node_t* ptr = bgjobs->head;
        for (int i = 0; i < bgjobs->length; i++)
        {
            if (((bgentry_t*)(ptr->value))->pid == pid)
            {
                recent = (bgentry_t*) removeByIndex(bgjobs, i);
                break;
            }
            ptr = ptr->next;
        }
        if (recent == NULL)
        {
            printf(PID_ERR);
            return;
        }
    }
    
    // prints the cmdline
    printf("%s\n", recent->job->line);
    pid_t wait_result = waitpid(recent->pid, exit_status, 0);
    if (wait_result < 0) {
        printf(WAIT_ERR);
        exit(EXIT_FAILURE);
    }
    free_job(recent->job);
    free(recent);
}

void killAllChild(List_t* bgjobs)
{
    if (bgjobs == NULL) { return; }
    node_t* ptr = bgjobs->head;
    for (int i = 0; i < bgjobs->length; i++)
    {
        bgentry_t* val = (bgentry_t*) (ptr->value);
        kill(val->pid, SIGKILL);
        fprintf(stdout, BG_TERM, val->pid, val->job->line);
        free_job(val->job);
        free(val);
        ptr = ptr->next;
    }
}

void print_rd_err()
{
    fprintf(stderr, RD_ERR);
}

int countRedirectionFiles(job_info* job)
{
    int n = 0;
    proc_info* proc = job->procs;
    for (int i = 0; i < job->nproc; i++)
    {
        if (proc->in_file != NULL) { ++n; }
        if (proc->out_file != NULL) { ++n; }
        if (proc->err_file != NULL) { ++n; }
        if (proc->outerr_file != NULL) { ++n; }
        proc = proc->next_proc;
    }
    return n;
}

int checkDuplicateFile(char** fileNames, char* nextFile, int* index)
{
    for (int i = 0; i < *index; i++)
    {
        if (strcmp(fileNames[i], nextFile) == 0)
        {
            return 0;
        }
    }
    fileNames[*index] = nextFile;
    ++(*index);
    return 1;
}

int checkRedirection_pipe(job_info* job)
{
    int numOfFiles = countRedirectionFiles(job);
    if (numOfFiles == 0) { return 1; }
    char** fileNames = malloc(numOfFiles * sizeof(char*));
    int fileNames_index = 0;
    int result = 1;
    proc_info* proc = job->procs;
    for (int i = 0; i < job->nproc; i++)
    {
        if (((proc->in_file != NULL) && ((i != 0) || (checkDuplicateFile(fileNames, proc->in_file, &fileNames_index) == 0)))
        || ((proc->out_file != NULL) && ((i != (job->nproc - 1)) || (checkDuplicateFile(fileNames, proc->out_file, &fileNames_index) == 0)))
        || ((proc->err_file != NULL) && (checkDuplicateFile(fileNames, proc->err_file, &fileNames_index) == 0))
        || ((proc->outerr_file != NULL) && ((i != (job->nproc - 1)) || (checkDuplicateFile(fileNames, proc->outerr_file, &fileNames_index) == 0))))
        {
            result = 0;
            break;
        }
        proc = proc->next_proc;
    }
    free(fileNames);
    return result;
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

void piping(job_info* job, List_t* bgjobs)
{
    // before creating pipes, check redirections
    if (checkRedirection_pipe(job) == 0)
    {
        print_rd_err();
        return;
    }

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
        if (child_pids[i] < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        if (child_pids[i] == 0)
        {
            // in child
			int redir_result = redirection(proc);
			if (redir_result == -1) {
				free_job(job);  
    			validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated
				exit(EXIT_FAILURE);
			}
            // close all pipes not used by child
            for (int j = 0; j < totalPipes; j++)
            {
                if (i == totalPipes || j != i)
                {
                    // close not used write pipe
                    close(pipes[j][1]);
                }
                if (i == 0 || j != i-1)
                {
                    // close not used read pipe
                    close(pipes[j][0]);
                }
            }
            if (i > 0) 
            {
                dup2(pipes[i-1][0], STDIN_FILENO); // redirect pipe to stdin
            }
            if (i < totalPipes)
            {
                dup2(pipes[i][1], STDOUT_FILENO); // redirect stdout to next pipe
            }
            int exec_result = execvp(proc->cmd, proc->argv);
            if (exec_result < 0)
            {
                printf(EXEC_ERR, proc->cmd);
                free_job(job);  
                validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated
                exit(EXIT_FAILURE);
            }
        }
        proc = proc->next_proc;
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
        bgjob->pid = child_pids[totalChild-1];  // use the pid of the last child for simplicity
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
            pid_t wait_result = waitpid(child_pids[i], &exit_status, 0);
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
