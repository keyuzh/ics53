#ifndef HELPERS_2_H
#define HELPERS_2_H

// Declare all helper functions for hw2 in this file
#include "hw2.h"

int dateFormatCheck(char* str);
int dateRangeCheck(char* str);
int dateCheck(char* str);

int my_strlen(char* str, const char end);
void my_strcpy(char* dest, char** src, int length);

char* getNextField(char** org);

char* joinNames(char* first, char* last);
char* getLastName(char* str, char** end_str);
char* getFirstName(char* str);
int searchName(char* str, char* criterion);

int strFullMatch(char* left, char* right);
int strStartswith(char* str, char* prefix);

int my_isalpha(char c);
int my_isdigit(char c);

void skipToNextWord(char** ptr);

int strKeywordMatch(char* toSearch, char* keyword);

void printTitle(char* title, void* fp, int flag);

int allDigits(char* str);

#endif