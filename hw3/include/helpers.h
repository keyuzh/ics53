// A header file for helpers.c
// Declare any additional functions in this file

#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "icssh.h"
#include "linkedList.h"

void command_cd(const char* path);

void sigusr2_handler(int sig);
void sigchld_handler(int sig);

int bgentry_tComparer(void* entry1, void* entry2);

void reapTerminatedChild(List_t* bgjobs);

void command_bglist(List_t* bgjobs);

void killAllChild(List_t* bgjobs);

int redirection(proc_info* proc);

#endif