#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdarg.h>

enum retType{
    SUCCESS = 0,
    MEMORY_ERROR,
    FILE_ERROR,
    NULL_ERROR,
    NO_SUCH_FUNC
};

enum userState{
    LOGINED,
    ADMIN_LOGINED,
    UNLOGINED,
    SANCTIONED
};

struct User
{
    char state;
    char *login;
    int attempts;
};


int dynamicReadline(char* str){
    if(!str){
        return NULL_ERROR;
    }
    size_t size = 32, len = 0;
    str = (char*)malloc(size * sizeof(char));
    if(!str){
        return MEMORY_ERROR;
    }
    char letter;
    while((letter = getc(stdin)) != '\n'){
        if(len == size){
            size *= 2;
            char* buf = (char*)malloc(sizeof(char) * size);
            if(!buf){
                free(str);
                return MEMORY_ERROR;
            }
            memcpy(buf, str, size/2);
            free(str);
            str = buf;
        }
        str[len++] = letter;
    }
    return SUCCESS;
}


int stringToWords(char* string, char** strings, int* amount){
    if(!string || !strings || !amount){
        return NULL_ERROR;
    }
    (*amount) = 0;
    size_t size = 1;
    strings = (char**)malloc(size * sizeof(char*));
    if(!strings){
        free(string);
        (*amount) = -1;
        return MEMORY_ERROR;
    }
    char* str = strtok(string, " ");
    do
    {
        int wordLen = strlen(str);
        char* save = (char*)malloc((wordLen + 1) * sizeof(char));
        if(!save){
            free(save);
            for (size_t i = 0; i < size; i++)
            {
                free(strings[i]);
            }
            free(string);
            (*amount) = -1;
            return MEMORY_ERROR;
        }

        memcpy(save, str, wordLen);
        save[wordLen] = '\0';

        strings[(*amount)++] = save;
    } while ((str = strtok(NULL, " ")) != NULL);
    return SUCCESS;
}

typedef int (*callback)(int, char**);

int reg(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int log(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int time(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int date(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int howmuch(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int logout(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int sanctions(int argumentNumber, char** strings){
    if(argumentNumber < 0 || strings == NULL){
        return NULL_ERROR;
    }
    return SUCCESS;
}

int commandHandler(char** strings, struct User* user, int wordAmount){
    char *commands[] = {"register", "login", "time", "date", "howmuch", "logout", "sanctions"};
    callback funcs[] = {reg, log, time, date, howmuch, logout, sanctions};
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
