#include "enums.h"
#include "commands.h"
#include "db.h"
#include "user.h"

#include <time.h>
#include <ctype.h>


int reg(int argumentNumber, char** strings, struct User* user){
    if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(!strcmp(strings[0], "NU")){
        return NU_IS_FORBIDDEN;
    }
    int ret = registerToDb("db.txt", strings[1], strings[2], -1);
    return ret;
}

int logn(int argumentNumber, char** strings, struct User* user){
    if(user->state == LOGINED || user->state == ADMIN_LOGINED){
        return ALREADY_LOGINED;
    }
    else if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(!strcmp(strings[0], "NU")){
        return NU_IS_FORBIDDEN;
    }
    int ret = loginDB("db.txt", strings[1], strings[2], &(*user));
    return ret;
}

int timeGet(int argumentNumber, char** strings, struct User* user){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current time: %02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return SUCCESS;
}

int date(int argumentNumber, char** strings, struct User* user){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current date: %02d:%02d:%02d\n", timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year + 1900);
    return SUCCESS;
}

int howmuch(int argumentNumber, char** strings, struct User* user){
    

    if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(strlen(strings[2]) != 2){
        printf("Unknown key %s!\n", strings[2]);
        return NO_MESSAGE_RETURN;
    }

    time_t now;
    time(&now);
    
    time_t tmp = 0;
    struct tm* parsed_timeinfo = localtime(&tmp);

    int day = -1, month = -1, year = -1;
    for (size_t i = 0; i < strlen(strings[1]); i++)
    {
        if(strings[1][i] != '/' && !isdigit(strings[1][i])){
            return DATE_ERROR;
        }
    }
    
    sscanf(strings[1], "%d/%d/%d", &day, &month, &year);
    year -= 1900;
    
    if(day > 31 || month > 12 || day < 0 || month < 0){
        return DATE_ERROR;
    }


    parsed_timeinfo->tm_mon = month;
    parsed_timeinfo->tm_year = year;
    parsed_timeinfo->tm_mday = day;


    

    time_t parsed_secs = mktime(&parsed_timeinfo);

    time_t delta = abs(parsed_secs - delta);


    switch (strings[2][1])
    {
    case 's':
        printf("The difference in seconds is %d\n", delta);
        break;
    case 'm':
        printf("The difference in minutes is %d\n", delta / 60);
        break;
    case 'h':
        printf("The difference in hours is %d\n", delta / 3600);
        break;
    case 'y':
        printf("The difference in years is %d\n", delta / 60 / 60 / 24 / 364);
        break;
    default:
        printf("Unknown key %s!\n", strings[2]);
        break;
    }

    return SUCCESS;
}

int logout(int argumentNumber, char** strings, struct User* user){
    if(user->state == UNLOGINED){
        return NOT_LOGINED;
    }
    user->attempts = -1;
    free(user->login);
    user->login = NULL;
    user->state = UNLOGINED;
    return SUCCESS;
}

int sanctions(int argumentNumber, char** strings, struct User* user){
    if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(user->state != ADMIN_LOGINED){
        return NO_PERMISSION;
    }
    if(findInDb("db.txt", strings[1]) != CONSIST){
        return NOT_CONSIST;
    }

    for (size_t i = 0; i < strlen(strings[2]); i++)
    {
        if(strings[2][i] != '-' && !isdigit(strings[2][i])){
            return MUST_BE_INT;
        }
    }
    
    int ret = changeSanctions("db.txt", strings[1], atoi(strings[2]));

    return ret;
}