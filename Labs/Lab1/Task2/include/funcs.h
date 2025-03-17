#ifndef FUNCS_H
#define FUNCS_H


int searchInFile(char* fileNAme, char* strToFind, FILE* output);
int find(int argc, char* argv[], char* strToFind);
int isInt(const char* str);
int newFileName(char* sourceFileName, char** destFileName, int iteration);
int copyFile(char* fileName, int iteration);
int copyN(int argc, char* argv[]);
int mask(FILE* f, const char* mask, int* answer);
int xorN(FILE* f, int N, unsigned long long int* xorRes);
int areEqualFiles(const char* file1, const char* file2);

#endif 