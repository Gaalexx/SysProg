#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include "enums.h"
#include "user.h"
#include "iohelper.h"
#include "db.h"

int encryptPassword(const char *password, char **encrypted_password);
int comparePasswords(const char *password, const char *hashed_password, int *compare_res);
int registerToDb(const char* dbName, const char* userLogin, const char* userPassword, const int sanctions);
int loginDB(const char* dbName, const char* userLogin, const char* userPassword, struct User* user);
int findInDb(const char* dbName, const char* userLogin);
int changeSanctions(const char* dbName, const char* userLogin, const int newPermission);


#endif 