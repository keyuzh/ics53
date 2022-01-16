#include "hw2.h"
#include "linkedlist.h"
#include "helpers2.h"
#include <string.h> // Not allowed in your HW implemention!!

int main() {

    char* str1 = "Mystery";  	   // Static string
    char str2[] = "Classic"; 	   // Array without size, edittable
    char str3[12] = "Self Help";   // Array with size, uninitialized bytes set to 0 
    char str4[4] = "Self Help";    // Array with size, truncated initialization (no null terminator!) DON'T USE THIS AS A STRING!!!

    char* str5 = calloc(5,1);  // 5 bytes of space initialized to 0
    // strcpy(str5,str4);         // String.h funcitions are not allowed in your HW implemention!! Create your own versions with pointers

    char* str6 = "Self";

    printf("\n******Compare Tests******\n");    
    printf("Mystery vs Classic, genreCompartor returns %d\n", genreComparator(str1, str2));
    printf("Classic vs Mystery, genreCompartor returns %d\n", genreComparator(str2, str1));
    printf("Classic vs Classic, genreCompartor returns %d\n", genreComparator(str2, str2));
    printf("Self Help vs Self, genreCompartor returns %d\n", genreComparator(str3, str6));

    printf("Mystery vs Classic, strcmp returns %d\n", strcmp(str1, str2));
    printf("Classic vs Mystery, strcmp returns %d\n", strcmp(str2, str1));
    printf("Classic vs Classic, strcmp returns %d\n", strcmp(str2, str2));
    printf("Self Help vs Self,  strcmp returns %d\n", strcmp(str3, str6));


    printf("\n\n******Deleter Test******\n");
    genreDeleter(str5);
    if(str5 != NULL)
        printf("OOPS!");

    //What happens if you try to delete str1 or str2? Why? Does it impact anything?
    //genreDeleter(str1);
    //genreDeleter(str2);
    
    printf("\n\n******getGenres Tests******\n");
    list_t* genre1 = getGenres("Fiction|Mystery|Classic\n");
    printf("\n***Classic->Fiction->Mystery***\n");
    PrintLinkedList(genre1, stdout); 

    list_t* genre2 = getGenres("Juvenile|Fiction|Young Adult|Family|Mystery|Classic\n");
    printf("\n***Classic->Family->Fiction->Juvenile->Mystery->Young Adult***\n");
    PrintLinkedList(genre2, stdout); 

    printf("\n\n***Error creating list***\n");
    list_t* genre3 = getGenres("Juvenile||Classic\n");
    if(genre3)
        printf("OOPS!\n");
    else
        printf("No list created!\n");



    printf("\n\n******FindInList Tests******\n");
    printf("\n***Search for Non-Fiction in List Classic->Fiction->Mystery***\n");
    node_t* item = FindInList(genre1, "Non-Fiction"); 
    if(item)
        printf("OOPS!\n");
    else
        printf("Non-Fiction not found in list\n");

    printf("\n***Search for Young Adult in List Classic->Family->Fiction->Juvenile->Mystery->Young Adult***\n");
    node_t* item2 = FindInList(genre2, "Young Adult"); 
    if(item2)
        printf("Young Adult found in list\n");
    else
        printf("OOPS!\n");



    printf("\n\n******DestroyList Tests******\n");
    printf("\n***Delete List 1***\n");
    DestroyList(&genre1);

    if(genre1)
        printf("OOPS!\n");
    else
        printf("genre1 list deleted\n");

    //This should print nothing!
    PrintLinkedList(genre1, stderr);

    printf("\n***Delete List 2***\n");
    DestroyList(&genre2);

    if(genre2)
        printf("OOPS!\n");
    else
        printf("genre2 list deleted\n");

    //This should print nothing!
    PrintLinkedList(genre2, stderr);


    // Check Valgrind! All memory should be free!

    printf("\n***Part 3***\n");
    char* goodbook1 = "The Complete Works of William Shakespeare,185326895,William,Shakespeare,08/04/1997,Drama|Fiction|Classic\n";
    char* goodbook2 = "Think Like a Programmer,1593274246,V.,Anton Spraul,08/12/2012,Tech Help|Textbook\n";
    char* goodbook3 = "\"Linear Algebra: Theory, Intuition, Code\",9083136604,Mike,Cohen,02/01/2021,Textbook\n";

    char* badbook1str = "The Complete Works of William Shakespeare,185326895,William,Shakespeare,08/04/2222,Drama|Fiction|Classic\n";
    char* badbook2str = "The Complete Works of William Shakespeare,,William,Shakespeare,08/04/1997,Drama|Fiction|Classic\n";
    char* badbook3str = "The Complete Works of William Shakespeare,185326895,William,Shakespeare,08/04/1997,Drama||Classic\n";

    book_t* book1 = createBook(goodbook1);
    book_t* book2 = createBook(goodbook2);
    book_t* book3 = createBook(goodbook3);
    book_tPrinter(book1, stdout, 1);
    book_tPrinter(book2, stdout, 1);
    book_tPrinter(book3, stdout, 1);

    book_tDeleter(book1);
    book_tDeleter(book2);
    book_tDeleter(book3);

    book_t* badbook1 = createBook(badbook1str);
    book_t* badbook2 = createBook(badbook2str);
    book_t* badbook3 = createBook(badbook3str);
    if (badbook1 != NULL) printf("OOPS");
    else printf("badbook1 are NULL\n");
    if (badbook2 != NULL) printf("OOPS");
    else printf("badbook2 are NULL\n");
    if (badbook3 != NULL) printf("OOPS");
    else printf("badbook3 are NULL\n");
    

    printf("\n******END OF TEST******\n");

    return 0;
}
