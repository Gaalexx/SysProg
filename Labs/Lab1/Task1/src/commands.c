#include "enums.h"
#include "commands.h"
#include "db.h"
#include "user.h"
#include "dateFuncs.h"

#include <time.h>
#include <ctype.h>




int reg(int argumentNumber, char** strings, struct User* user){
    if(user->state == LOGINED || user->state == ADMIN_LOGINED){
        return ALREADY_LOGINED;
    }
    if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(!strcmp(strings[0], "NU")){
        return NU_IS_FORBIDDEN;
    }
    else if(strlen(strings[2]) > 6){
        return WRONG_LEN;
    }
    for (size_t i = 0; i < (size_t)strlen(strings[2]); i++)
    {
        if(!isdigit(strings[2][i])){
            return MUST_BE_INT;
        }
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
    (void)argumentNumber;
    (void)strings;
    (void)user;
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current time: %02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return SUCCESS;
}

int date(int argumentNumber, char** strings, struct User* user){
    (void)argumentNumber;
    (void)strings;
    (void)user;

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current date: %02d:%02d:%02d\n", timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year + 1900);
    return SUCCESS;
}



int howmuch(int argumentNumber, char** strings, struct User* user){
    (void)user;

    if(argumentNumber != 3){
        return WRONG_AMOUNT_OF_ARGS;
    }
    else if(strlen(strings[2]) != 2){
        printf("Unknown key %s!\n", strings[2]);
        return NO_MESSAGE_RETURN;
    }
  

    time_t now;
    time(&now);
    
    struct tm* timeInfoNow = localtime(&now);

    
    int day = -1, month = -1, year = -1;
    for (size_t i = 0; i < strlen(strings[1]); i++)
    {
        if(strings[1][i] != '/' && !isdigit(strings[1][i])){
            return DATE_ERROR;
        }
    }
    
    sscanf(strings[1], "%d/%d/%d", &day, &month, &year);
    
    if(day > 31 || month > 12 || day < 0 || month < 0){
        return DATE_ERROR;
    }
    else if(day > daysInMonth(month, year)){
        return DATE_ERROR;
    }

    struct Date d1, d2;
    
    d1.year = 1900 + timeInfoNow->tm_year;
    d1.month = timeInfoNow->tm_mon + 1;
    d1.day = timeInfoNow->tm_mday + 1;

    d2.year = year;
    d2.month = month;
    d2.day = day;

    if(d2.year < 1970){
        printf("There might be errors if you have input a year that is less than 1970\n");
    }
    switch (strings[2][1])
    {
    case 'd':
        printf("Delta in days: %lld\n", (daysDelta(d1, d2) == 0) ? daysDelta(d1, d2) - 1 : daysDelta(d1, d2));
        break;
    case 's':
        printf("Delta in seconds: %lld\n", (daysDelta(d1, d2) == 0) ? daysDelta(d1, d2) - 1 : daysDelta(d1, d2) * 24 * 60 * 60);
        break;
    case 'm':
        printf("Delta in minutes: %lld\n", (daysDelta(d1, d2) == 0) ? daysDelta(d1, d2) - 1 : daysDelta(d1, d2) * 24 * 60);
        break;
    case 'h':
        printf("Delta in hours: %lld\n", (daysDelta(d1, d2) == 0) ? daysDelta(d1, d2) - 1 : daysDelta(d1, d2) * 24);
        break;
    case 'y':
        printf("Delta in years: %d\n", yearsDelta(d1, d2));
        break;
    default:
        printf("Unknown key %s!\n", strings[2]);
        break;
    }

    return SUCCESS;
}

int logout(int argumentNumber, char** strings, struct User* user){
    (void)argumentNumber;
    (void)strings;
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
    
    char* confirmation;
    printf("Input 12345 to confirm sanctions... ");
    int ret = dynamicReadline(&confirmation);

    if(ret != SUCCESS){
        return ret;
    }

    if(strlen(confirmation) != 5 || strcmp(confirmation, "12345")){
        return DENIED;
    }
    
    free(confirmation);

    ret = changeSanctions("db.txt", strings[1], atoi(strings[2]));

    

    return ret;
}

int help(int argumentNumber, char** strings, struct User* user){
    (void)argumentNumber;
    (void)strings;
    if(user == NULL){
        return NULL_ERROR;
    }
    else if(user->state != ADMIN_LOGINED){
        printf("These are avaliable commands:\n1) register <login> <password>\n2) login <login> <password>\n"
               "3) time\n4) date\n5) howmuch <date> <flag> (-s - in seconds; -h - in hours; -m - in minutes; -y - in years)\n"
               "6) logout\n");
    }
    else{
        printf("These are avaliable commands:\n1) register <login> <password>\n2) login <login> <password>\n"
               "3) time\n4) date\n5) howmuch <date> <flag> (-s - in seconds; -h - in hours; -m - in minutes; -y - in years)\n"
               "6) logout\nADMIN MENU:\n1*) sanctions <login> <attempts>\n");
    }
    return SUCCESS;
}