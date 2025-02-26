#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enums.h"

int dynamicReadline(char* str);
int stringToWords(char* string, char** strings, int* amount);
int fDynamicReadline(char* str, FILE* file);

#endif 