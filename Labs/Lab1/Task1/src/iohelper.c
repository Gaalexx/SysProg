#include <stdio.h>
#include "enums.h"
#include "iohelper.h"

int fDynamicReadline(char** strr, FILE* file){
    if(!strr){
        return NULL_ERROR;
    }
    else if(!file){
        return FILE_ERROR;
    }


    size_t size = 32, len = 0;
    (*strr) = (char*)malloc(size * sizeof(char));
    if(!(*strr)){
        return MEMORY_ERROR;
    }
    char letter;
    while((letter = fgetc(file)) != '\n' && letter != EOF){
        if(len + 1 == size){
            size *= 2;
            char* buf = (char*)malloc(sizeof(char) * size);
            if(!buf){
                free((*strr));
                return MEMORY_ERROR;
            }
            memcpy(buf, (*strr), size/2);
            free(*strr);
            (*strr) = buf;
        }
        (*strr)[len++] = letter;
        (*strr)[len] = '\0';
    }
    if(letter == EOF){
        return END_OF_FILE;
    }
    return SUCCESS;
}

int dynamicReadline(char** strr){
    if(!strr){
        return NULL_ERROR;
    }
    size_t size = 32, len = 0;
    (*strr) = (char*)malloc(size * sizeof(char));
    if(!(*strr)){
        return MEMORY_ERROR;
    }
    char letter;
    while((letter = getc(stdin)) != '\n'){
        if(len + 1 == size){
            size *= 2;
            char* buf = (char*)realloc((*strr), sizeof(char) * size);
            if(!buf){
                free((*strr));
                return MEMORY_ERROR;
            }
            free((*strr));
            (*strr) = buf;
        }
        (*strr)[len++] = letter;
        (*strr)[len] = '\0';
    }
    return SUCCESS;
}


int stringToWords(char* str, char*** strings, int* amount){
    
    if(!str || !(*strings) || !amount){
        return NULL_ERROR;
    }
    (*amount) = 0;
    size_t size = 1;
    (*strings) = (char**)malloc(size * sizeof(char*));
    if(!(*strings)){
        (*amount) = -1;
        return MEMORY_ERROR;
    }
    char* ptr = strtok(str, " \n\t");
    if(ptr == NULL){
        (*amount) = -1;
        return NULL_ERROR;
    }
    do
    {
        int wordLen = strlen(ptr);
        char* save = (char*)malloc((wordLen + 1) * sizeof(char));
        if(!save){
            free(save);
            for (size_t i = 0; i < size; i++)
            {
                free((*strings)[i]);
            }
            free(str);
            (*amount) = -1;
            return MEMORY_ERROR;
        }

        memcpy(save, ptr, wordLen);
        save[wordLen] = '\0';

        if((*amount) == (int)size){
            size *= 2;
            char ** nw_stirngs = realloc((*strings), sizeof(char*) * size);
            if(!nw_stirngs){
                for (size_t i = 0; i < size / 2; i++)
                {
                    free((*strings)[i]);
                }
                free((*strings));
                return MEMORY_ERROR;
            }
            (*strings) = nw_stirngs;
        }
        (*strings)[(*amount)++] = save;
    } while ((ptr = strtok(NULL, " \n\t")) != NULL);
    return SUCCESS;
}