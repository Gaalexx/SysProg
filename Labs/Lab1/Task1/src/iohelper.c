#include <stdio.h>
#include "include/enums.h"
#include "include/iohelper.h"

int fDynamicReadline(char* str, FILE* file){
    if(!str){
        return NULL_ERROR;
    }
    else if(!file){
        return FILE_ERROR;
    }


    size_t size = 32, len = 0;
    str = (char*)malloc(size * sizeof(char));
    if(!str){
        return MEMORY_ERROR;
    }
    char letter;
    while((letter = getc(stdin)) != '\n' && letter != EOF){
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
    if(letter == EOF){
        return END_OF_FILE;
    }
    return SUCCESS;
}

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