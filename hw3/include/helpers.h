// A header file for helpers.c
// Declare any additional functions in this file

#ifndef HELPERS_H
#define HELPERS_H

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "debug.h"
#include "icssh.h"
#include "linkedList.h"
#include "pwd.h"

#define ICS53_ASCII_ART \
"68b                                     \n"\
"Y89                                     \n"\
"___   ____     ____    _______   ____   \n"\
"`MM  6MMMMb.  6MMMMb  ,MMMMMMM  6MMMMb  \n"\
" MM 6M'   Mb MM'    ` dM       MM'  `Mb \n"\
" MM MM    `' YM.      MP             MM \n"\
" MM MM        YMMMMb  M'            .M9 \n"\
" MM MM            `Mb MMMMMMb    MMMM   \n"\
" MM YM.   d9 L    ,MM      `Mb      `Mb \n"\
"_MM_ YMMMM9  MYMMMM9        MM       MM \n"\
"                            MM       MM \n"\
"                      MM.  ,M9 MM.  ,M9 \n"\
"                       YMMMM9   YMMMM9  \n"

char* customized_shell_prompt(int* offset);
char* set_customized_shell(char* prompt, int offset);
char* get_username();

void command_print_ascii_art();

void command_cd(const char* path);

void sigusr2_handler(int sig);
void sigchld_handler(int sig);

int bgentry_tComparer(void* entry1, void* entry2);

void reapTerminatedChild(List_t* bgjobs, int* exit_status);

void command_bglist(List_t* bgjobs);

void killAllChild(List_t* bgjobs);

int redirection(proc_info* proc);

void piping(job_info* job, List_t* bgjobs);

#endif