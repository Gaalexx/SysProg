#include <stdio.h>
#include <string.h>

#include "enums.h"
#include "commands.h"
#include "handlers.h"

int commandHandler(char** strings, struct User* user, int wordAmount){
    if(wordAmount < 0 || strings == NULL || user == NULL){
        return NULL_ERROR;
    }
    char *commands[] = {"register", "login", "time", "date", "howmuch", "logout", "sanctions"};
    callback funcs[] = {reg, logn, time, date, howmuch, logout, sanctions};
    int ret = NO_SUCH_FUNC;
    for (size_t i = 0; i < 7; i++)
    {
        if(!strcmp(strings[0], commands[i])){
            ret = funcs[i](wordAmount, strings);
            break;
        }
    }
    return ret;
}

void errorHandler(int ret){
    switch (ret)
    {
    case MEMORY_ERROR:
        printf("Memory error!\n");
        break;
    case FILE_ERROR:
        printf("File error!\n");
        break;
    case NULL_ERROR:
        printf("Nul error!\n");
        break;
    default:
        break;
    }
}