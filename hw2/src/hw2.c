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
    char* c1 = str1;
    char* c2 = str2;
    while (*c1 != '\0' || *c2 != '\0')
    {
        if (*c1 != *c2)
        {
            return *c1 > *c2 ? 1 : -1;
        }
        ++c1;
        ++c2;
    }
    return 0;
}

void genrePrinter(void* data, void* fp, int flag) {
    if (flag)
    { // pretty print
        fprintf(fp, "%s,", (char*)data); 
    }
    else 
    { // debug print
        fprintf(fp, "%s", (char*)data); 
    }
}

void genreDeleter(void* data) {
    // free() can handle nullptr
    free(data);
    data = NULL; // how to make original ptr NULL ??
}

list_t* getGenres(char* str) {
    if (str == NULL || *str == '\0')
    {
        // nullptr OR empty string
        return NULL;
    }
    list_t* ll = CreateList(genreComparator, genrePrinter, genreDeleter);
    char* ptr = str;
    while (ptr != NULL)
    {
        GenreGetter gg = getNextGenre(ptr);
        if (gg.current == NULL)
        {
            // empty genre
            DestroyList(&ll);
            // free(gg);
            return NULL;
        }
        InsertInOrder(ll, gg.current);
        ptr = gg.next;
        // free(gg);
    }
    return ll;
}

// Part 2 Generic Linked List functions
node_t* FindInList(list_t* list, void* token) {
    if (list == NULL || token == NULL)
    {
        return NULL;
    }
    node_t* ptr = list->head;
    while (ptr != NULL)
    {
        if (!(list->comparator(ptr->data, token)))
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void DestroyList(list_t** list)  {
    if (list == NULL || *list == NULL) 
    {
        return;
    }
    node_t* toDelete;
    node_t* ptr = (*list)->head;
    while (ptr != NULL)
    {
        (*list)->deleter(ptr->data);
        toDelete = ptr;
        ptr = ptr->next;
        free(toDelete);
    }
    free(*list);
    *list = NULL;
}




// Part 3 Functions to implement
void book_tPrinter(void* data, void *fp, int flag) {
    if (data == NULL) { return; }
    book_t* book = data;
    if (flag)
    {
        // pretty print
        // prints ISBN, month, day
        fprintf(fp, "%u\t%02d\t%02d\t", book->ISBN, book->pubDate.month, book->pubDate.day);
    }
    fprintf(fp, "%04d\t%s\t", book->pubDate.year, book->name);
    printTitle(book->title, fp, flag);
    // print genres
    list_t* genres = book->genres;
    node_t* ptr = genres->head;
    while (ptr != NULL) {
        genrePrinter(ptr->data, fp, 0);
        ptr = ptr->next;
        if (ptr != NULL) { fprintf(fp, ","); }
    }
    fprintf(fp, "\n");
}

int book_tISBNAscComparator(void* lhs, void* rhs) {
    unsigned int left = ((book_t*)lhs)->ISBN;
    unsigned int right = ((book_t*)rhs)->ISBN;
    if (left == right)
    {
        return 0;
    }
    return (left > right) ? 1 : -1;
}

int book_tISBNDescComparator(void* lhs, void* rhs) {
    return -1 * book_tISBNAscComparator(lhs, rhs);
}

void book_tDeleter(void* data) {
    book_t* book = data;
    free(book->name);
    free(book->title);
    DestroyList(&(book->genres));
    free(book);
}

book_t* createBook(char* line) {
    char *bookTitle, *bookISBNstr, *bookFirstName, *bookLastName, *bookPublishDate;
    bookTitle = bookISBNstr = bookFirstName = bookLastName = bookPublishDate = NULL;
    // TODO: error handling when the input didn't have enough fields
    if (line == NULL) { return NULL; }
    char* reader = line;
    book_t* book = malloc(sizeof(book_t));

    bookTitle = getNextField(&reader);
    bookISBNstr = getNextField(&reader);
    bookFirstName = getNextField(&reader);
    bookLastName = getNextField(&reader);
    bookPublishDate = getNextField(&reader);
    list_t* genres = getGenres(reader);
    if ((getDate(bookPublishDate, &(book->pubDate)) == 0)
        || (genres == NULL) || (bookTitle == NULL) || (bookISBNstr == NULL) 
        || (bookFirstName == NULL) || (bookLastName == NULL))
    {
        DestroyList(&genres);
        free(bookTitle);
        free(book);
        book = NULL;
    }
    else
    {
        book->name = joinNames(bookFirstName, bookLastName);
        book->title = bookTitle;
        book->ISBN = atoi(bookISBNstr);
        book->genres = genres;
    }
    free(bookISBNstr);
    free(bookFirstName);
    free(bookLastName);
    free(bookPublishDate);
    return book;
}




// Part 4 Functions to implement
int bookMatch(book_t* curBook, search_t* criterion) {
    if (curBook == NULL || criterion == NULL) { return -1; }
    int match = 0;
    if (criterion->name != NULL)
    {
        match += searchName(curBook->name, criterion->name);
    }
    if (criterion->keyword != NULL)
    {
        match += strSubstringMatch(curBook->name, criterion->keyword);
        match += strSubstringMatch(curBook->title, criterion->keyword);
    }
    if (criterion->ISBN)
    {
        match += (curBook->ISBN == criterion->ISBN);
    }
    if (criterion->pubDate.day != 0 && criterion->pubDate.month != 0 && criterion->pubDate.year != 0)
    {
        match += (cmpDate(criterion->pubDate, curBook->pubDate) <= 0);
    }
    if (criterion->genre != NULL)
    {
        match += (FindInList(curBook->genres, criterion->genre) != NULL);
    }
    return (match > 0);
}

void PrintNLinkedList(list_t* list, FILE* fp, int NUM) {
    if (NUM < 0 || list == NULL) { return; }
    node_t* ptr = list->head;
    int numPrinted = 0;
    while (ptr != NULL && (NUM == 0 || numPrinted < NUM))
    {
        list->printer(ptr->data, fp, 1);
        ptr = ptr->next;
        ++numPrinted;
    }
}
