#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdarg.h>

#include "enums.h"
#include "user.h"
#include "iohelper.h"
#include "handlers.h"


int main(){
    char* str;
    dynamicReadline(&str);

    char** strings;
    int cnt;

    stringToWords(str, &strings, &cnt);

    for (size_t i = 0; i < cnt; i++)
    {
        printf("%s ", strings[i]);
        free(strings[i]);
    }
    free(strings);
    free(str);
    return 0;
}


/* int main(int argc, char const *argv[])
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
        if(ret != SUCCESS){
            errorHandler(ret);
            
        }
        ret = stringToWords(buffer, &strings, &wordAmount);
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
} */
