// Declare all helper functions for hw1 in this file
#include "helpers1.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char getMode(char *argv[])
{
    return argv[1][1];
}

char* modeLFormat(char* msg, int s)
{
    char* result = malloc(sizeof(*msg));

    char* org = msg;
    char* res = result;

    while (*org != 0)
    {
        if (isalpha(*org) || (s != 0 && (*org == ' ')))
        {
            *res = tolower(*org);
            ++res;
        }
        ++org;
    }
    *res = '\0';
    
    // printf("MESSAGE:%s\n", result);
    return result;
}

void skipToNextLine()
{
    char t;
    while (1)
    {
        t = tolower(getchar());
        if (t == EOF) { return; }
        printf("%c", t);
        if (t == '\n') { return; }
    }
}

unsigned int modeLprocess(char* msg, Options op)
{
    char temp;
    unsigned int count = 0;
    while (1)
    {
        temp = tolower(getchar());
        if (temp == EOF)
        {
            break;
        }
        if ((*msg != '\0' && *msg != ' ' && temp == *msg)
           || (*msg == ' ' && strchr(",!?.", temp) != NULL && op.s != 0))
        {
            temp = toupper(temp);
            printf("%c", temp);
            if (*msg == ' ')
            {
                printf("%*c", op.s, ' ');
            }
            if (op.o)
            {
                skipToNextLine();
            }
            ++msg;
            ++count;
        }
        else
        {
            printf("%c", temp);
        }
    }
    return count;
}

int isModeS(int argc, char* argv[])
{
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-S") == 0)
        {
            return atoi(argv[i+1]);
        }
    }
    return 0;
}

int isModeOther(int argc, char* argv[], const char mode)
{
    char target[2] = "-";
    // strcat(target, mode);
    target[1] = mode;
    // printf("%s\n", target);
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], target) == 0)
        {
            return 1;
        }
    }
    return 0;
}