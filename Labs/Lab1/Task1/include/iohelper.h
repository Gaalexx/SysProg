#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enums.h"

int dynamicReadline(char** strr);
int stringToWords(char* string, char*** strings, int* amount);
int fDynamicReadline(char** strr, FILE* file);

#endif 