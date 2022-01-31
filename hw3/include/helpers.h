// A header file for helpers.c
// Declare any additional functions in this file

#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "icssh.h"

void command_cd(const char* path);
void sigusr2_handler(int sig);

#endif