#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdarg.h>

#include "include/enums.h"
#include "include/user.h"
#include "include/iohelper.h"
#include "include/handlers.h"

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

        ret = dynamicReadline(buffer);
        if(ret != SUCCESS){
            errorHandler(ret);
            
        }
        ret = stringToWords(buffer, strings, &wordAmount);
        if(ret != SUCCESS){
            free(buffer);
            errorHandler(ret);
        }

        

        
        //обработка команд


        for (size_t i = 0; i < wordAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
        free(buffer);
    }
    
    return 0;
}
