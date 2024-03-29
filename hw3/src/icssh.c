#include "icssh.h"
#include "helpers.h"
#include "linkedList.h"
#include <readline/readline.h>
volatile sig_atomic_t CHECK_BACKGROUND_JOBS;

int main(int argc, char* argv[]) {
	int exec_result;
	int exit_status;
	pid_t pid;
	pid_t wait_result;
	char* line;
	// signal handlers
	signal(SIGUSR2, sigusr2_handler);
	sigset_t mask_child, prev;
	sigemptyset(&mask_child);
	sigaddset(&mask_child, SIGCHLD);
	signal(SIGCHLD, sigchld_handler);
	List_t bgjobs = {NULL, 0, &bgentry_tComparer};
	CHECK_BACKGROUND_JOBS = 0;

#ifdef GS
    rl_outstream = fopen("/dev/null", "w");
#endif

	// Setup segmentation fault handler
	if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
		perror("Failed to set signal handler");
		exit(EXIT_FAILURE);
	}

	// custom shell prompt
	int custom_shell_offset;
	char* custom_shell = NULL;
	#ifdef DEBUG
		custom_shell = customized_shell_prompt(&custom_shell_offset);
	#endif

    // print the prompt & wait for the user to enter commands string
	while ((line = readline(set_customized_shell(custom_shell, custom_shell_offset))) != NULL) {
		// check if sigchld flag is set
		if (CHECK_BACKGROUND_JOBS)
		{
			reapTerminatedChild(&bgjobs, &exit_status);
			CHECK_BACKGROUND_JOBS = 0;
		}

        // MAGIC HAPPENS! Command string is parsed into a job struct
        // Will print out error message if command string is invalid
		job_info* job = validate_input(line);
        if (job == NULL) { // Command was empty string or invalid
			free(line);
			continue;
		}

        //Prints out the job linked list struture for debugging
        #ifdef DEBUG   // If DEBUG flag removed in makefile, this will not longer print
            debug_print_job(job);
        #endif

		// example built-in: exit
		if (strcmp(job->procs->cmd, "exit") == 0) {
			killAllChild(&bgjobs);  // kill all child before exit
			deleteList(&bgjobs);
			// Terminating the shell
			free(line);
			free_job(job);
            validate_input(NULL);   // calling validate_input with NULL will free the memory it has allocated
			free(custom_shell);
            return 0;
		}
		if (strcmp(job->procs->cmd, "ascii53") == 0) {
			command_print_ascii_art();
			free(line);
			free_job(job);
			continue;
		}
		if (strcmp(job->procs->cmd, "cd") == 0) {
			// cd
			char* path;
			if (job->procs->argc == 1) {
				// no dir supplied
				path = getenv("HOME");
			} else {
				path = job->procs->argv[1];
			}
			command_cd(path);
			free(line);
			free_job(job);
			continue;
		}
		if (strcmp(job->procs->cmd, "estatus") == 0) {
			//estatus
			fprintf(stdout, "%d\n", WEXITSTATUS(exit_status));
			free(line);
			free_job(job);
			continue;
		}
		if (strcmp(job->procs->cmd, "bglist") == 0) {
			// bglist
			command_bglist(&bgjobs);
			free(line);
			free_job(job);
			continue;
		}
		if (strcmp(job->procs->cmd, "fg") == 0) {
			// fg
			pid_t fg_pid = job->procs->argc > 1 ? atoi(job->procs->argv[1]) : -1;
			command_fg(&bgjobs, &exit_status, fg_pid);
			free(line);
			free_job(job);
			continue;
		}

		// piping
		if (job->nproc > 1)
		{
			free(line);
			piping(job, &bgjobs);
			continue;
		}

		// example of good error handling!
		if ((pid = fork()) < 0) {
			perror("fork error");
			exit(EXIT_FAILURE);
		}
		if (pid == 0) {  //If zero, then it's the child process
            //get the first command in the job list
		    proc_info* proc = job->procs;
			int redir_result = redirection(proc);
			if (redir_result == -1) {
				free_job(job);  
				free(line);
    			validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated
				exit(EXIT_FAILURE);
			}
			exec_result = execvp(proc->cmd, proc->argv);
			if (exec_result < 0) {  //Error checking
				printf(EXEC_ERR, proc->cmd);
				
				// Cleaning up to make Valgrind happy 
				// (not necessary because child will exit. Resources will be reaped by parent)
				free_job(job);  
				free(line);
    			validate_input(NULL);  // calling validate_input with NULL will free the memory it has allocated

				exit(EXIT_FAILURE);
			}
		} else if (job->bg) {
			// background task
			bgentry_t* bgjob = malloc(sizeof(bgentry_t));
			bgjob->job = job;
			bgjob->pid = pid;
			time(&(bgjob->seconds));
			insertInOrder(&bgjobs, bgjob);
		} else {
            // As the parent, wait for the foreground job to finish
			// block sigchld 
			sigprocmask(SIG_BLOCK, &mask_child, &prev);
			wait_result = waitpid(pid, &exit_status, 0);
			sigprocmask(SIG_SETMASK, &prev, NULL);
			if (wait_result < 0) {
				printf(WAIT_ERR);
				exit(EXIT_FAILURE);
			}
			free_job(job);  // if a foreground job, we no longer need the data
		}
		free(line);
	}
	
	deleteList(&bgjobs);
	free(custom_shell);
    // calling validate_input with NULL will free the memory it has allocated
    validate_input(NULL);

#ifndef GS
	fclose(rl_outstream);
#endif
	return 0;
}
