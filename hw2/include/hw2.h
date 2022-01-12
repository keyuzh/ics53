// DO NOT MODIFY THIS FILE
// Any additions should be placed in helpers2.h

#ifndef HW_2_H
#define HW_2_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "linkedlist.h"

typedef struct date_t
{
	unsigned int month:4;
	unsigned int day:5;
	unsigned short int year;
} Date;

typedef struct
{
	char *name;                       // Author name in format "lastname, firstname"
	char *title;                      // Book Title
	unsigned int ISBN;                // ISBN number of the book
	Date pubDate;                     // Publication Date
	list_t* genres;                   // Linked List of genre(s) 
} book_t;

typedef struct
{
	char *name;                       // Author name
	char *keyword;                    // Keyword
	unsigned int ISBN;                // ISBN number of the book
	Date pubDate;                     // Publication Date
	char* genre;                      // Genre
} search_t;

// Part 1 Functions to implement
int getDate(char* str, Date* myDate);
int cmpDate(const Date date1, const Date date2);

// Part 2 Functions to implement
int genreComparator(void* str1, void* str2);
void genrePrinter(void* data, void* fp, int flag);
void genreDeleter(void* data);
list_t* getGenres(char* str);

node_t* FindInList(list_t* list, void* token);
void DestroyList(list_t** list);

// Part 3 Functions to implement
void book_tPrinter(void* data, void *fp, int flag);
int book_tISBNAscComparator(void* lhs, void* rhs);
int book_tISBNDescComparator(void* lhs, void* rhs);
void book_tDeleter(void* data);
book_t* createBook(char* line);

// Part 4 Functions to implement
int bookMatch(book_t* curBook, search_t* criterion);
void PrintNLinkedList(list_t* list, FILE* fp, int NUM);

#endif
