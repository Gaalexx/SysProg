#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include "enums.h"

struct Date {
    long year;
    long month;
    long day;
};

int dateCmp(struct Date d1, struct Date d2);
int dateSwap(struct Date* d1, struct Date* d2);
int yearsDelta(struct Date curDate, struct Date otherDate);
int isLeapYear(int year);
int daysInMonth(int month, int year);
int daysRemainsForEnd(int year, int month, int day);
long long int daysDelta(struct Date curDate, struct Date otherDate);

#endif