// Define all helper functions for hw2 in this file
#include "helpers2.h"
#include <stdlib.h>

// checks whether a str is a valid date format
// format: mm/dd/yyyy
int dateFormatCheck(char* str) 
{
    char* ptr = str;
    for (int i = 0; i <= 10; i++)
    {
        switch (i)
        {
        case 10:
            if (*ptr != '\0')
            {
                return 0;
            }
            break;
        case 2:
        case 5:
            if (*ptr != '/')
            {
                return 0;
            }
            break;
        default:
            if (*ptr < '0' || *ptr > '9')
            {
                return 0;
            }
            break;
        }
        ++ptr;
    }
    return 1;
}

// format: mm/dd/yyyy
int dateRangeCheck(char* str) 
{
    int month = atoi(str);
    int day   = atoi(str+3);
    int year  = atoi(str+6);
    return !(month < 1 || month > 12 || day < 1 || day > 31 || year > 2022);
}

int dateCheck(char* str) 
{
    return (dateFormatCheck(str) && dateRangeCheck(str));
}

int getGenreLength(char* str)
{
    int len = 0;
    while (*str != '\n' && *str != '|' && *str != '\0')
    {
        ++len;
        ++str;
    }
    return len;
}

void copyGenre(GenreGetter* gg, char* src)
{
    char* dest = gg->current;
    while (*src != '\n' && *src != '|')
    {
        *dest = *src;
        ++dest;
        ++src;
    }
    if (*src == '\n')
    {
        gg->next = NULL;
    }
    else 
    {
        gg->next = ++src;
    }
}

GenreGetter getNextGenre(char* str)
{
    GenreGetter gg;
    int len = getGenreLength(str);
    if (len == 0)
    {
        gg.current = NULL;
        return gg;
    }
    gg.current = malloc(len + 1);
    copyGenre(&gg, str);
    return gg;
}
