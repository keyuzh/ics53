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

GenreGetter getNextGenre(char* str);

#endif