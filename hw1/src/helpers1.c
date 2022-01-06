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
    
    // printf("%s\n", result);
    return result;
}

unsigned int modeLprocess(char* msg, int s)
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
        else if (*msg != '\0' && *msg != ' ' && temp == *msg)
        {
            temp = toupper(temp);
            ++msg;
            ++count;
            printf("%c", temp);
        }
        else if (*msg == ' ' && strchr(",!?.", temp) != NULL && s != 0)
        {
            ++msg;
            ++count;
            printf("%c", temp);
            printf("%*c", s, ' ');
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