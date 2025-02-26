#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include "enums.h"
#include "commands.h"



int commandHandler(char** strings, struct User* user, int wordAmount);
void errorHandler(int ret);

#endif 