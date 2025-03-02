#ifndef COMMANDS_H
#define COMMANDS_H

#include "enums.h"
#include "user.h"

typedef int (*callback)(int argumentNumber, char** strings, struct User* user);

int reg(int argumentNumber, char** strings, struct User* user);
int logn(int argumentNumber, char** strings, struct User* user);
int timeGet(int argumentNumber, char** strings, struct User* user);
int date(int argumentNumber, char** strings, struct User* user);
int howmuch(int argumentNumber, char** strings, struct User* user);
int logout(int argumentNumber, char** strings, struct User* user);
int sanctions(int argumentNumber, char** strings, struct User* user);
int help(int argumentNumber, char** strings, struct User* user);
#endif 