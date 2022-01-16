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
    *dest = '\0';
    if (*src == '\n')
    {
        gg->next = NULL;
    }
    else 
    {
        gg->next = ++src;
    }
}

GenreGetter* getNextGenre(char* str)
{
    GenreGetter* gg = malloc(sizeof(GenreGetter));
    int len = getGenreLength(str);
    if (len == 0)
    {
        gg->current = NULL;
        return gg;
    }
    gg->current = malloc(len+1);
    copyGenre(gg, str);
    return gg;
}

int my_strlen(char* str, char end)
{
    int len = 0;
    int betweenQuotes = 0;
    while (*str != end || betweenQuotes)
    {
        if (*str == '"')
        {
            betweenQuotes = 1 - betweenQuotes;
        }
        ++len;
        ++str;
    }
    return len;
}

void my_strcpy(char* dest, char** src, int length)
{
    // copy the next input field for the given length, move the src pointer to the
    // start of next input field
    char* read = *src;
    for (int i = 0; i < length; i++)
    {
        *dest = *read;
        ++dest;
        ++read;
    }
    *dest = '\0';
    *src = ++read;
}

char* getNextField(char** org)
{
    if (org == NULL || *org == NULL) { return NULL; }
    int nextFieldLength = my_strlen(*org, ',');
    char* str = malloc(nextFieldLength + 1);
    my_strcpy(str, org, nextFieldLength);
    if (*str == '\0')
    {
        free(str);
        *org = NULL;
        return NULL;
    }
    return str;
}

char* joinNames(char* first, char* last)
{
    char end = '\0';
    char* result = malloc(my_strlen(first, end) + my_strlen(last, end) + 2);
    char* dest = result;
    // copy last name
    while (*last != '\0')
    {
        *dest = *last;
        ++dest;
        ++last;
    }
    *(dest++) = ',';
    while (*first != '\0')
    {
        *dest = *first;
        ++dest;
        ++first;
    }
    *dest = '\0';
    return result;
}
