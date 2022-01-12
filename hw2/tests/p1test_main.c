#include <stdio.h>
#include <stdlib.h>

#include "helpers2.h"
#include "hw2.h"
#include "linkedlist.h"

int main(void) {


    printf("\n****Create Invalid Date*****\n");
    char date_string2[] = "17/01/2000";  // edittable copy of string
    Date  d;   // d currently contains GARBAGE values

    printf("Date is: %s\n", date_string2);
    int success = getDate(date_string2, &d);

    if(success)
        printf("OOPS!!!\n");
    else
        printf("Date string is invalid\n");



    printf("\n****Create Valid Date*****\n");
    char* date_string = "05/21/1980";  //reference to static string

    printf("Date is: %s\n", date_string);
    success = getDate(date_string, &d);

    if(success)
        printf("month:%u, day:%u, year:%u\n", d.month, d.day, d.year);
    else
        printf("OOPS!!!\n");


    printf("\n****Compare Dates*****\n");
    Date  f = {1,1,2022};   // f is initialized

    success = cmpDate(d, f);

    if(success < 0)
        printf("Date d is before Date f\n");
    else if (success)
        printf("Date d is the same as Date f\n");
    else
        printf("Date d is after Date f\n");

    return 0;
}
