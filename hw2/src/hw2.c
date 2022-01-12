// Keyu Zhang
// keyuz4

#include "hw2.h"
#include "helpers2.h"
#include "linkedlist.h"

// Part 1 Functions to implement
/**
* Inserts the parsed values into myDate from str. 
* Returns 0 upon success and 1 upon error. 
* Error includes NULL pointer(s), invalid str input format, month value outside [1,12], day
* value [1,31], or year > 2022. Checking for valid day within a specific month or for leap year * is not performed. 
*
* @param str Pointer to string representing date
* @param myDate Pointer to instance of Date struct to fill
* @return 1 on success, 0 on error
**/
int getDate(char* str, Date* myDate) {
    // format: mm/dd/yyyy
    if (str == NULL)
    {
        return 0;
    }
    if (!dateCheck(str))
    {
        return 0;
    }
    int month = atoi(str);
    int day   = atoi(str+3);
    int year  = atoi(str+6);
    myDate->day = day;
    myDate->month = month;
    myDate->year = year;
    return 1;
}

int cmpDate(const Date date1, const Date date2) {
    if (date1.year != date2.year)
    {
        return date1.year > date2.year ? 1 : -1;
    }
    if (date1.month != date2.month)
    {
        return date1.month > date2.month ? 1 : -1;
    }
    if (date1.day != date2.day)
    {
        return date1.day > date2.day ? 1 : -1;
    }
    return 0;
}




// Part 2 Functions to implement
int genreComparator(void* str1, void* str2) {
    return -999;
}

void genrePrinter(void* data, void* fp, int flag) {

}

void genreDeleter(void* data) {

}

list_t* getGenres(char* str) {
    return NULL;
}

// Part 2 Generic Linked List functions
node_t* FindInList(list_t* list, void* token) {
    return NULL;
}

void DestroyList(list_t** list)  {

}




// Part 3 Functions to implement
void book_tPrinter(void* data, void *fp, int flag) {
 
}

int book_tISBNAscComparator(void* lhs, void* rhs) {
    return -999;
}

int book_tISBNDescComparator(void* lhs, void* rhs) {
    return -999;
}

void book_tDeleter(void* data) {

}

book_t* createBook(char* line) {
    return NULL;
}




// Part 4 Functions to implement
int bookMatch(book_t* curBook, search_t* criterion) {
    return -999;
}

void PrintNLinkedList(list_t* list, FILE* fp, int NUM) {
}