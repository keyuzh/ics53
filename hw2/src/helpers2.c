// Define all helper functions for hw2 in this file
#include "helpers2.h"
// #include <stdlib.h>

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
    return !(month < 1 || month > 12 || day < 1 || day > 31 || year < 0 || year > 2022);
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
    while (*src != '\n' && *src != '|' && *src != '\0')
    {
        *dest = *src;
        ++dest;
        ++src;
    }
    // *dest = '\0';
    if (*src == '\n' || *src == '\0')
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
    // GenreGetter* gg = malloc(sizeof(GenreGetter));
    GenreGetter gg;
    int len = getGenreLength(str);
    if (len == 0)
    {
        gg.current = NULL;
        return gg;
    }
    gg.current = calloc(len+1, sizeof(char));
    copyGenre(&gg, str);
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
    char* result = malloc(my_strlen(first, end) + my_strlen(last, end) + 3);
    char* dest = result;
    // copy last name
    while (*last != '\0')
    {
        *dest = *last;
        ++dest;
        ++last;
    }
    *(dest++) = ',';
    *(dest++) = ' ';
    while (*first != '\0')
    {
        *dest = *first;
        ++dest;
        ++first;
    }
    *dest = '\0';
    return result;
}

char* getLastName(char* str, char** end_str)
{
    char end = ',';
    // extract last name from the string
    int len = my_strlen(str, end);
    char* lastName = malloc(len + 1);

    char* to = lastName;
    int betweenQuotes = 0;
    while (*str != end || betweenQuotes)
    {
        if (*str == '"')
        {
            betweenQuotes = 1 - betweenQuotes;
        }
        *to = *str;
        ++to;
        ++str;
    }
    *to = '\0';
    *end_str = str+2;
    return lastName;
}

char* getFirstName(char* str)
{
    char end = '\0';
    // extract first name from the string
    int len = my_strlen(str, end);
    char* name = malloc(len + 1);

    char* to = name;
    int betweenQuotes = 0;
    while (*str != end || betweenQuotes)
    {
        if (*str == '"')
        {
            betweenQuotes = 1 - betweenQuotes;
        }
        *to = *str;
        ++to;
        ++str;
    }
    *to = '\0';
    return name;
}

int searchName(char* str, char* criterion)
{
    char* firstNameSubstring;
    char* lastName = getLastName(str, &firstNameSubstring);
    int matchLastName = strFullMatch(lastName, criterion);
    free(lastName);
    char* firstName = getFirstName(firstNameSubstring);
    int matchFirstName = strFullMatch(firstName, criterion);
    int result = matchFirstName + matchLastName;
    free(firstName);
    return result;
}

int strFullMatch(char* left, char* right)
{
    while (*left != '\0' || *right != '\0')
    {
        if (*left != *right)
        {
            return 0;
        }
        ++left;
        ++right;
    }
    return 1;
}

int strStartswith(char* str, char* prefix)
{
    while (*prefix != 0)
    {
        if (*str != *prefix)
        {
            return 0;
        }
        ++str;
        ++prefix;
    }
    return 1;
}

int strSubstringMatch(char* toSearch, char* subString)
{
    while (*toSearch != '\0')
    {
        if (strStartswith(toSearch, subString))
        {
            return 1;
        }
        ++toSearch;
    }
    return 0;
}

int my_isalpha(char c)
{
    return (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'));
}

int my_isdigit(char c)
{
    return ('0' <= c && c <= '9');
}


void skipToNextWord(char** ptr)
{
    while (my_isalpha(**ptr))
    {
        ++(*ptr);
    }
    while ((**ptr != 0) && (!my_isalpha(**ptr)))
    {
        ++(*ptr);
    }
}

int strKeywordMatch(char* toSearch, char* keyword)
{
    // only match full words, no substring
    char* key = keyword;
    while (*toSearch != '\0')
    {
        while (*key != 0)
        {
            if (*key != *toSearch)
            {
                skipToNextWord(&toSearch);
                key = keyword;
                break;
            }
            ++toSearch;
            ++key;
        }
        if ((*key == 0) && (!my_isalpha(*toSearch)))
        {
            return 1;
        }
        // ++toSearch;
    }
    return 0;
}

int compareDates(int year1, int month1, int day1, int year2, int month2, int day2)
{
    // return 1 if date 1 is on or after date 2
    if (year1 > year2)
    {
        return 1;
    }
    else if (year1 == year2)
    {
        if (month1 > month2)
        {
            return 1;
        }
        else if (month1 == month2)
        {
            return (day1 >= day2);
        }
    }
    return 0;
}

void printTitle(char* title, void* fp, int flag)
{
    // print the title of book without quotation marks
    int hasQuotes = strStartswith(title, "\"");
    // flag == 1, hasQuotes == 0  -> add quotes
    // flag == 1, hasQuotes == 1  -> dont add quotes
    // flag == 0, hasQuotes == 0  -> dont add quotes
    // flag == 0, hasQuotes == 1  -> remove quotes
    // printf("\nflag: %d, hasQuotes: %d\n", flag, hasQuotes);
    // printf("title: %s\n", title);
    if (flag == 0 && hasQuotes)
    {
        // remove quotes
        char* ptr = ++title;
        while (*ptr != '"')
        {
            fprintf(fp, "%c", *ptr);
            ++ptr;
        }
    }
    else if (flag && hasQuotes == 0)
    {
        fprintf(fp, "\"%s\"\t", title);
    }
    else
    {
        fprintf(fp, "%s\t", title);
    }
}

int criterionAllNull(search_t* c)
{
    return (c->name == NULL && c->keyword == NULL && c->ISBN == 0 
        && c->pubDate.day == 0 && c->pubDate.month == 0 && c->pubDate.year == 0
        && c->genre == NULL);
}
