#ifndef COMMANDS_H
#define COMMANDS_H

#include "enums.h"

typedef int (*callback)(int, char**);

int reg(int argumentNumber, char** strings);
int logn(int argumentNumber, char** strings);
int time(int argumentNumber, char** strings);
int date(int argumentNumber, char** strings);
int howmuch(int argumentNumber, char** strings);
int logout(int argumentNumber, char** strings);
int sanctions(int argumentNumber, char** strings);

#endif 