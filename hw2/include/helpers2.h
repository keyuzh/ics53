#ifndef HELPERS_2_H
#define HELPERS_2_H

// Declare all helper functions for hw2 in this file
#include "hw2.h"

typedef struct {
    char* current;
    char* next;
} GenreGetter;


int dateFormatCheck(char* str);
int dateRangeCheck(char* str);
int dateCheck(char* str);
int getGenreLength(char* str);

GenreGetter getNextGenre(char* str);

int my_strlen(char* str, const char end);
void my_strcpy(char* dest, char** src, int length);
char* getNextField(char** org);
char* joinNames(char* first, char* last);

char* getLastName(char* str, char** end_str);
char* getFirstName(char* str);
int searchName(char* str, char* criterion);
int strFullMatch(char* left, char* right);
int strStartswith(char* str, char* prefix);
int strSubstringMatch(char* toSearch, char* subString);
int compareDates(int year1, int month1, int day1, int year2, int month2, int day2);
void printTitle(char* title, void* fp, int flag);
int criterionAllNull(search_t* c);


#endif