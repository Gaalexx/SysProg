#include "enums.h"
#include "dateFuncs.h"
#include <stdlib.h>
#include <math.h>



int dateCmp(struct Date d1, struct Date d2){
    if(d1.year - d2.year < 0){
        return -1;
    }
    else if(d1.year - d2.year > 0){
        return 1;
    }
    else if(d1.month - d2.month < 0){
        return -1;
    }
    else if(d1.month - d2.month > 0){
        return 1;
    }
    else if(d1.day - d2.day < 0){
        return -1;
    }
    else if(d1.day - d2.day > 0){
        return 1;
    }
    else{
        return 0;
    }
}

int dateSwap(struct Date* d1, struct Date* d2){
    if(!d1 || !d2){
        return NULL_ERROR;
    }
    struct Date tmp;
    tmp.year = d1->year;
    tmp.month = d1->month;
    tmp.day = d1->day;

    d1->month = d2->month;
    d1->year = d2->year;
    d1->day = d2->day;

    d2->year = tmp.year;
    d2->month = tmp.month;
    d2->day = tmp.day;

    return SUCCESS;
}


int yearsDelta(struct Date curDate, struct Date otherDate){
    if(dateCmp(curDate, otherDate) == 0){
        return 0;
    }
    int delta = abs((int)curDate.year - (int)otherDate.year) - 1;
    if(delta == -1){
        return 0;
    }
    else if(curDate.month > otherDate.month && delta != -1){
        delta += 1;
    }
    else if(curDate.month == otherDate.month && curDate.day > otherDate.day && delta != -1){
        delta += 1;
    }
    return delta;
}



int isLeapYear(int year) {
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        return 1;
    }
    return 0;
}

int daysInMonth(int month, int year) {
    if (month == 2) { 
        return isLeapYear(year) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) { 
        return 30;
    } else { 
        return 31;
    }
}

int daysRemainsForEnd(int year, int month, int day) {
    int days = 0;

    days += daysInMonth(month, year) - day;

    for (int m = month + 1; m <= 12; m++) {
        days += daysInMonth(m, year);
    }

    return days;
}


long long int daysDelta(struct Date curDate, struct Date otherDate){
    long long int delta = 0;
    int cmp;
    if((cmp = dateCmp(curDate, otherDate)) == 0){
        return 0;
    }
    else if(cmp == -1){
        dateSwap(&curDate, &otherDate);
    }

    /* delta += ((curDate.year != otherDate.year) ? (daysRemainsForEnd(otherDate.year, otherDate.month, otherDate.day)
     + ((isLeapYear(curDate.year)) ? 366 : 365)) - 
    daysRemainsForEnd(curDate.year, curDate.month, curDate.day) : 
    (daysRemainsForEnd(otherDate.year, otherDate.month, otherDate.day) - 
    daysRemainsForEnd(curDate.year, curDate.month, curDate.day))); */


    if (curDate.year != otherDate.year) {
        int leapDays = isLeapYear(curDate.year) ? 366 : 365;
        delta += 
        (daysRemainsForEnd(otherDate.year, otherDate.month, otherDate.day) 
        + 
        leapDays 
        - 
        daysRemainsForEnd(curDate.year, curDate.month, curDate.day));
    } else {
        delta += (daysRemainsForEnd(otherDate.year, otherDate.month, otherDate.day) - daysRemainsForEnd(curDate.year, curDate.month, curDate.day));
    }
    
    for (int i = otherDate.year + 1; i < curDate.year; i++)
    {
        delta += isLeapYear(i) ? 366 : 365;
    }
    
    return delta;
}