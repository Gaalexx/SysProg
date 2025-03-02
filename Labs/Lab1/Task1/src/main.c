#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdarg.h>

#include "enums.h"
#include "user.h"
#include "iohelper.h"
#include "handlers.h"
#include "db.h"


int main(int argc, char const *argv[])
{
    printf("Welcome to the time checker! You must login to start working with this software, input your login and password.\nIf you haven't logined yet, use the command <register>.\n");
    struct User user;
    user.state = UNLOGINED;
    user.attempts = -1;
    user.login = NULL;

    for (;;)
    {
        char *buffer, **strings;
        int ret, wordAmount;

        ret = dynamicReadline(&buffer);
        if(strlen(buffer) == 0){
            continue;
        }
        if(ret != SUCCESS){
            errorHandler(ret);
            continue;
        }
        ret = stringToWords(buffer, &strings, &wordAmount);
        if(ret != SUCCESS){
            free(buffer);
            errorHandler(ret);
            continue;
        }

        ret = commandHandler(strings, &user, wordAmount);
        if(ret != SUCCESS && ret != EXIT){
            errorHandler(ret);
            printf("Problem!\n");
        }
        else if(ret == EXIT){
            for (size_t i = 0; i < wordAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            free(buffer);
            return 0;
        }
        


        for (size_t i = 0; i < wordAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
        free(buffer);
    }
    
    return 0;
}