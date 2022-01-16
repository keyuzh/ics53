#ifndef HELPERS_2_H
#define HELPERS_2_H

// Declare all helper functions for hw2 in this file

typedef struct {
    char* current;
    char* next;
} GenreGetter;


int dateFormatCheck(char* str);
int dateRangeCheck(char* str);
int dateCheck(char* str);

GenreGetter* getNextGenre(char* str);

int my_strlen(char* str, const char end);
void my_strcpy(char* dest, char** src, int length);
char* getNextField(char** org);
char* joinNames(char* first, char* last);

#endif