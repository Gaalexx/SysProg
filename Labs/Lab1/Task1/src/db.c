#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include "enums.h"
#include "user.h"
#include "iohelper.h"
#include "db.h"


int encryptPassword(const char *password, char **encrypted_password)
{

    char *salt = crypt_gensalt_ra(NULL, 0, NULL, 0);
    if (!salt)
    {
        return CRYPT_ERROR;
    }

    void *enc_ctx = NULL;
    int enc_cxt_sz = 0;
    char *tmp_encrypted_password = crypt_ra(password, salt, &enc_ctx, &enc_cxt_sz);

    if (tmp_encrypted_password == NULL)
    {
        free(salt);
        return CRYPT_ERROR;
    }

    *encrypted_password = (char *)calloc((strlen(tmp_encrypted_password) + 1), sizeof(char));

    strcpy(*encrypted_password, tmp_encrypted_password);

    free(enc_ctx);
    free(salt);
    return SUCCESS;
}

int comparePasswords(const char *password, const char *hashed_password, int *compare_res)
{

    int cmp_res = 0;
    (void)cmp_res;

    void *enc_ctx = NULL;
    int enc_cxt_sz = 0;

    char *hashed_entered_password = crypt_ra(password, hashed_password, &enc_ctx, &enc_cxt_sz);
    if (!hashed_entered_password)
    {
        return CRYPT_ERROR;
    }

    *compare_res = strcmp(hashed_password, hashed_entered_password);

    free(enc_ctx);
    return SUCCESS;
}


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

    char* encryptedPassword;
    int ret = encryptPassword(userPassword, &encryptedPassword);
    if(ret != SUCCESS){
        fclose(in);
        return ret;
    }
    fprintf(in, "%s %s %d\n", userLogin, encryptedPassword, sanctions);

    fclose(in);

    return SUCCESS;
}

int changeSanctions(const char* dbName, const char* userLogin, const int newPermission){

    FILE* f = fopen("db.txt", "r+");
    if(!f){
        return FILE_ERROR;
    }

    char* line;
    int ret = fDynamicReadline(&line, f);
    
    if(ret != SUCCESS){
        fclose(f);
        return ret;
    }
    int retSTW;


    long long fileIndex = 0;
    do
    {
        int numberOfWords;
        char** words;
        int len_line_beg = strlen(line) + 1;
        retSTW = stringToWords(line, &words, &numberOfWords);
        if(retSTW == MEMORY_ERROR){
            fclose(f);
            free(line);
        }
        else if(!strcmp(words[0], "NU")){
            for (size_t i = 0; i < (size_t)numberOfWords; i++)
            {
                free(words[i]);
            }
            free(words);
            fileIndex += len_line_beg;
            free(line);
            continue;
        }

        if(!strcmp(words[0], userLogin)){
            fseek(f, fileIndex, SEEK_SET);
            fputs("NU ", f);
            fclose(f);

            f = fopen(dbName, "a");
            if(!f){
                for (size_t i = 0; i < (size_t)numberOfWords; i++)
                {
                    free(words[i]);
                }
                free(words);
                free(line);
                return FILE_ERROR;
            }

            fprintf(f, "%s %s %d\n", words[0], words[1], newPermission);


            for (size_t i = 0; i < (size_t)numberOfWords; i++)
            {
                free(words[i]);
            }
            free(words);
            free(line);
            fclose(f);
            return SUCCESS;
        }


        for (size_t i = 0; i < (size_t)numberOfWords; i++)
        {
            free(words[i]);
        }
        free(words);
        fileIndex += len_line_beg;
        free(line);
    } while ((ret = fDynamicReadline(&line, f)) != END_OF_FILE);
    fclose(f);
    return NOT_CONSIST;
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
        retF = fDynamicReadline(&str, out);
        if(retF != SUCCESS && retF != END_OF_FILE){
            fclose(out);
            return retF;
        }

        char** strings;
        int ret = stringToWords(str, &strings, &stringsAmount);
        if(ret != SUCCESS){
            return ret; 
        }
        else if(!strcmp(strings[0], "NU")){
            free(str);
            for (size_t i = 0; i < (size_t)stringsAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            continue;
        }
        

        if(!strcmp(strings[0], userLogin)){
            int coinsidence;

            comparePasswords(userPassword, strings[1], &coinsidence);
            if(!coinsidence){
                if(iteration == 1){ //если это первый зарегистрированный пользователь, то это админ
                    user->state = ADMIN_LOGINED;
                }else{
                    user->state = LOGINED;
                }
    
                user->attempts = atoi(strings[2]);

                if(user->login != NULL){
                    free(user->login);
                }

                int lgn_len = strlen(strings[0]);
                char* lgn = malloc((lgn_len + 1) * sizeof(char));
                if(!lgn){
                    free(str);
                    for (size_t i = 0; i < (size_t)stringsAmount; i++)
                    {
                        free(strings[i]);
                    }
                    free(strings);
                    fclose(out);
                    return MEMORY_ERROR;
                }
                user->login = lgn;
                strcpy(user->login, strings[0]);
                lgn[lgn_len] = '\0';
                
    
                free(str);
                for (size_t i = 0; i < (size_t)stringsAmount; i++)
                {
                    free(strings[i]);
                }
                free(strings);
                fclose(out);
                return SUCCESS;
            }
            else{
                free(str);
                for (size_t i = 0; i < (size_t)stringsAmount; i++)
                {
                    free(strings[i]);
                }
                free(strings);
                fclose(out);
                return WRONG_PASSWORD;
            }
            
        }
        //основная работа


        free(str);
        for (size_t i = 0; i < (size_t)stringsAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
    }while(retF != END_OF_FILE);
    fclose(out);

    return LOGIN_FAILED;
}


int findInDb(const char* dbName, const char* userLogin){
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
        retF = fDynamicReadline(&str, out);
        if(retF != SUCCESS && retF != END_OF_FILE){
            fclose(out);
            return retF;
        }

        char** strings;
        int ret = stringToWords(str, &strings, &stringsAmount);
        if(ret != SUCCESS){
            return ret; //память в случае чего очищается в функции
        }
        else if(!strcmp(strings[0], "NU")){
            free(str);
            for (size_t i = 0; i < (size_t)stringsAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            continue;
        }

        if(!strcmp(strings[0], userLogin)){
            free(str);
            for (size_t i = 0; i < (size_t)stringsAmount; i++)
            {
                free(strings[i]);
            }
            free(strings);
            fclose(out);
            return CONSIST;
        }
        //основная работа


        free(str);
        for (size_t i = 0; i < (size_t)stringsAmount; i++)
        {
            free(strings[i]);
        }
        free(strings);
    }while(retF != END_OF_FILE);
    fclose(out);
    return NOT_CONSIST;
}