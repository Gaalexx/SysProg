#include <stdio.h>
#include <stdlib.h>
#include "include/enums.h"
#include "include/user.h"
#include "include/iohelper.h"

int registerToDb(const char* dbName, const char* userLogin, const char* userPassword, const int sanctions){
    if(!dbName || !userLogin || !userPassword){
        return NULL_ERROR;
    }
    

    //здесь проверка на то, что такой пользователь уже есть в базе данных
    if(findInDb(dbName, userLogin) == CONSIST){
        return ALREADY_EXIST;
    }

    FILE *in = fopen(dbName, "a");
    if(!in){
        return FILE_ERROR;
    }

    fprintf(in, "%s %s %d\n", userLogin, userPassword/*захэшировать*/, sanctions); //доделать хэширование пароля

    fclose(in);

    return SUCCESS;
}


int loginDB(const char* dbName, const char* userLogin, const char* userPassword, struct User* user){
    if(!dbName || !userLogin || !userPassword || !user){
        return NULL_ERROR;
    }

    FILE* out = fopen(dbName, "r");
    if(!out){
        return FILE_ERROR;
    }
    int retF = SUCCESS;

    int iteration = 0;

    do{
        iteration++;
        char* str;
        int stringsAmount;
        retF = fDynamicReadline(str, out);
        if(retF != SUCCESS || retF != END_OF_FILE){
            fclose(out);
            return retF;
        }

        char** strings;
        int ret = stringToWords(str, strings, &stringsAmount);
        if(ret != SUCCESS){
            return ret; //память в случае чего очищается в функции
        }

        if(!strcmp(strings[0], userLogin) && !strcmp(strings[1], userPassword/*захэшировать*/)){
            
            if(iteration == 1){ //если это первый зарегистрированный пользователь, то это админ
                user->state = ADMIN_LOGINED;
            }else{
                user->state = LOGINED;
            }

            user->attempts = atoi(strings[2]);
            strcpy(user->login, strings[0]);

            free(str);
            for (size_t i = 0; i < stringsAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            fclose(out);
            return SUCCESS;
        }
        //основная работа


        free(str);
        for (size_t i = 0; i < stringsAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
    }while(retF != END_OF_FILE);
    fclose(out);

    return LOGIN_FAILED;
}


int findInDb(char* dbName, char* userLogin){
    if(!dbName || !userLogin){
        return NULL_ERROR;
    }

    FILE* out = fopen(dbName, "r");
    if(!out){
        return FILE_ERROR;
    }
    int retF = SUCCESS;
    do{
        char* str;
        int stringsAmount;
        retF = fDynamicReadline(str, out);
        if(retF != SUCCESS || retF != END_OF_FILE){
            fclose(out);
            return retF;
        }

        char** strings;
        int ret = stringToWords(str, strings, &stringsAmount);
        if(ret != SUCCESS){
            return ret; //память в случае чего очищается в функции
        }

        if(!strcmp(strings[0], userLogin)){
            free(str);
            for (size_t i = 0; i < stringsAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            fclose(out);
            return CONSIST;
        }
        //основная работа


        free(str);
        for (size_t i = 0; i < stringsAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
    }while(retF != END_OF_FILE);
    fclose(out);
    return NOT_CONSIST;
}