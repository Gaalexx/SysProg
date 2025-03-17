#include <stdio.h>
#include <string.h>

#include "enums.h"
#include "commands.h"
#include "handlers.h"

int commandHandler(char** strings, struct User* user, int wordAmount){
    if(wordAmount < 0 || strings == NULL || user == NULL){
        return NULL_ERROR;
    }
    if(!strcmp(strings[0], "exit")){
        return EXIT;
    }
    char *commands[] = {"help", "register", "login", "logout", "time", "date", "howmuch", "sanctions"};
    callback funcs[] = {help, reg, logn, logout, timeGet, date, howmuch, sanctions};
    int ret = NO_SUCH_FUNC;
    for (size_t i = 0; i < 8; i++)
    {
        if(!strcmp(strings[0], commands[i])){
            if(user->state == UNLOGINED && i > 2){
                return NOT_LOGINED;
            }/* else if(i <= 2){
                ret = funcs[i](wordAmount, strings, &(*user));
            } */
            else if(user->state != UNLOGINED && i > 3){
                if(user->attempts != 0){
                    ret = funcs[i](wordAmount, strings, &(*user));
                }
                else{
                    return NO_ATTEMPTS;
                }
            }
            else if(user->state != UNLOGINED && i == 3){
                ret = funcs[i](wordAmount, strings, &(*user));
            }
            else{
                ret = funcs[i](wordAmount, strings, &(*user));
            }
            break;
        }
    }
    if(ret != NO_SUCH_FUNC && ret != EXIT && user->attempts > 0 && strcmp(strings[0], "login")){
        user->attempts--;
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
        printf("Null error!\n");
        break;
    case NO_SUCH_FUNC:
        printf("There is no such command!\n");
        break;
    case ALREADY_LOGINED:
        printf("You are already logined!\n");
        break;
    case NOT_LOGINED:
        printf("You are not logined!\n");
        break;
    case ALREADY_EXIST:
        printf("Such login already exists!\n");
        break;
    case LOGIN_FAILED:
        printf("Login failed! Try another time!\n");
        break;
    case WRONG_PASSWORD:
        printf("Wrong password!\n");
        break;
    case WRONG_AMOUNT_OF_ARGS:
        printf("Wrong amount of passwords for this command. Use 'help' to check avaliable commands and needed arguments!\n");
        break;
    case DATE_ERROR:
        printf("Date error. May be you have made a mistake in a date!\n");
        break;
    case UNKNOWN_KEY:
        printf("Unknown key for this function!\n");
        break;
    case NO_PERMISSION:
        printf("You have no permission to use this command!\n");
        break;
    case MUST_BE_INT:
        printf("The argument must be integer!\n");
        break;
    case NU_IS_FORBIDDEN:
        printf("Login 'NU' is forbidden!\n");
        break;
    case NO_ATTEMPTS:
        printf("You have run out of your attemps, try it in another session!\n");
        break;
    case WRONG_LEN:
        printf("The password mustn't be longer than 6 symbols!\n");
        break;
    case NOT_CONSIST:
        printf("There is no such login!\n");
        break;
    case DENIED:
        printf("Action denied!\n");
        break;
    default:
        break;
    }
    return;
}
