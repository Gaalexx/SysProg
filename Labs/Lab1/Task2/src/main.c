#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "iohelper.h"
#include "enums.h"


void error_handler(int ret){
    switch (ret)
    {
    case NO_ARGUMENTS_ERROR:
        printf("There must be at least 3 arguments! Input at least one path to the file you want to edit or analyse!\n");
        break;
    case FILE_ERROR:
        printf("The error with a file occured!\n");
        break;
    case NULL_ERROR:
        printf("Null error!\n");
        break;
    case MEMORY_ERROR:
        printf("Memory error occured!\n");
        break;
    case PIPE_ERROR:
        printf("Pipe error occured!\n");
        break;
    case FORK_ERROR:
        printf("Fork error occured!\n");
        break;
    case MUST_BE_INTEGER:
        printf("The N must be integer!\n");
        break;
    case MUST_BE_POSITIVE:
        printf("The N must be positive!\n");
        break;
    case WRONG_RANGE:
        printf("N must be in the range of [2, 6]\n");
        break;
    case MUST_NOT_BE_EQUAL_FOR_COPY:
        printf("Input files must not be equal for command copyN!\n");
    default:
        break;
    }
}


int searchInFile(char* fileNAme, char* strToFind, FILE* output){
    FILE* dtr = fopen(fileNAme, "r");
    if(!dtr || !strToFind || !output){ //поправить мб
        return NULL_ERROR;
    }
    int len = strlen(strToFind);
    //fprintf(output, "File %s:\n", fileNAme);
    char ch = ' ';
    int flag_eq = 0;
    int enter_cnt = 1, num_of_sym = 1, iteration = 0, found_strs = 0;
    do
    {
        ch = fgetc(dtr);
        iteration++;
        num_of_sym++;
        if(ch == '\n' || ch == '\r'){
            num_of_sym = 1;
            enter_cnt++;
        }
        
        if(strToFind[0] == ch){
            int n_iter = iteration;
            (void)n_iter;
            int i = 1;
            flag_eq = 1;
            while (ch != EOF || strToFind[i] != ch || strToFind[i] != '\0' || i != len)
            {
                ch = fgetc(dtr);
                //char str_i = strToFind[i];
                if(strToFind[i] != ch  && !(strToFind[i] == '\n' && ch == '\r')){ 
                    fseek(dtr, iteration, SEEK_SET);
                    flag_eq = 0;
                    break;
                }
                i++;
                if(i == len){
                    break;
                }
            }
            if(flag_eq){
                //fprintf(output, "\tString: %d\tNumber: %d\n", enter_cnt, num_of_sym - 1);
                found_strs++;
                //num_of_sym++;
                fseek(dtr, iteration, SEEK_SET);
                flag_eq = 0;
            }
            else{
                fseek(dtr, iteration, SEEK_SET);
            }
        }
    } while (ch != EOF);
    if(!found_strs){
        fprintf(output, "\tNo strings found\n");
    }
    else{
        fprintf(output, "%d coincidences in %s file\n", found_strs, fileNAme);
    }
    fclose(dtr);
    return SUCCESS;
}

int find(int argc, char* argv[], char* strToFind){
    FILE* sout = stdout;
    for (size_t i = 1; i < (size_t)argc - 2; i++)
    {
        pid_t p = fork();

        if(p < 0){
            return FORK_ERROR;
        }
        else if(p == 0){
            searchInFile(argv[i], strToFind, sout);
            exit(0);
        }
    }
    return SUCCESS;
}

int isInt(const char* str){
    for (size_t i = 0; i < strlen(str); i++)
    {
        if(!isdigit(str[i]) && (i != 0 && str[i] != '-')){
            return 0;
        }
    }
    return 1;
}

int newFileName(char* sourceFileName, char** destFileName, int iteration){
    if(!sourceFileName){
        return NULL_ERROR;
    }
    *destFileName = (char*)malloc((strlen(sourceFileName) + 11) * sizeof(char));
    if(!*destFileName){
        return MEMORY_ERROR;
    }

    char* sourceFileName_copy = (char*)malloc((strlen(sourceFileName) + 1) * sizeof(char));
    if(!sourceFileName_copy){
        free(*destFileName);
        return MEMORY_ERROR;
    }
    strcpy(sourceFileName_copy, sourceFileName);
    char* dataType = strrchr(sourceFileName_copy, '.');
    if(dataType != NULL){
        *dataType = '\0';
        dataType++;
    }

    sprintf(*destFileName, "%s_%d.%s", sourceFileName_copy, iteration, dataType);
    free(sourceFileName_copy);
    return SUCCESS;
}

int copyFile(char* fileName, int iteration){
    char* nFileName = NULL;
    int ret = newFileName(fileName, &nFileName, iteration);
    if(ret != SUCCESS){
        return ret;
    }
    FILE* fi = fopen(fileName, "rb");
    if(!fi){
        free(nFileName);
        return FILE_ERROR;
    }
    FILE* fo = fopen(nFileName, "wb");
    if(!fo){
        free(nFileName);
        fclose(fi);
        return FILE_ERROR;
    }

    char* buffer = (char*)malloc(sizeof(char) * BUFSIZ);
    if(!buffer){
        free(nFileName);
        fclose(fi);
        fclose(fo);
        return MEMORY_ERROR; 
    }
    size_t charsRead;
    do
    {
        charsRead = fread((void*)buffer, sizeof(char), BUFSIZ, fi);
        if(charsRead > 0){
            fwrite((void*)buffer, sizeof(char), charsRead, fo);
        }
    } while (charsRead == BUFSIZ);
    
    free(buffer);
    free(nFileName);

    return SUCCESS;
}

int copyN(int argc, char* argv[]){
    int N;
    if(!isInt(argv[argc - 1])){
        return MUST_BE_INTEGER;
    }
    N = atoi(argv[argc - 1]);
    if(N <= 0){
        return MUST_BE_POSITIVE;
    }
    for (size_t i = 1; i < (size_t)argc - 2; i++)
    {
        for (size_t j = 1; j < (size_t)N + 1; j++)
        {
            pid_t p = fork();

            if(p < 0){
                return FORK_ERROR;
            }
            else if(p == 0){
                int ret = copyFile(argv[i], j);
                (void)ret;
                exit(ret);
            }
        }
        
    }
    return SUCCESS;
}

int mask(FILE* f, const char* mask, int* answer){
    char* str;
    int ret = fDynamicReadline(&str, f);
    if(ret != SUCCESS){
        return ret;
    }
    int intMask = atoi(mask);
    (*answer) = 0;
    do
    {
        char* word = strtok(str, " \t\n");
        do
        {
            if(ret != SUCCESS){
                free(str);
                return ret;
            }
            if(isInt(word)){
                if((atoi(word) & intMask) == intMask){
                    ++(*answer);
                }
            }
        } while ((word = strtok(NULL, " \n\t")) != NULL);
        free(str);
        ret = fDynamicReadline(&str, f);
    } while (ret != END_OF_FILE);
    free(str);
    return SUCCESS;
}

int xorN(FILE* f, int N, unsigned long long int* xorRes){ //!!!!!!!протестировать!!!!!!!!
    if(!f){
        return FILE_ERROR;
    }
    else if(!xorRes){
        return NULL_ERROR;
    }


    size_t blockSizeBits = (size_t)pow((double)2, (double)N), bufferSize = 256;
    size_t blockSizeBytes = (blockSizeBits + 4) / 8;

    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * bufferSize);
    if(!buffer){
        return MEMORY_ERROR;
    }

    unsigned long long int curXor = 0, curBinary = 0;
    size_t retSize;
    (void)curBinary;(void)curXor;
    unsigned char* bufXor = (unsigned char*)calloc(blockSizeBytes, sizeof(unsigned char));
    if(!bufXor){
        free(buffer);
        return MEMORY_ERROR;
    }

    do
    {
        retSize = fread((void*)buffer, sizeof(unsigned char), bufferSize, f);
        if(retSize > 0){
            
            size_t ext = (retSize > blockSizeBytes) ? (retSize % blockSizeBytes) : blockSizeBytes - retSize;
            if(ext){
                for (size_t i = retSize + 1; i < retSize + ext; i++)
                {
                    buffer[i] = 0;
                }
            }
            for (size_t i = 0; i < retSize; i += blockSizeBytes)
            {
                for (size_t j = 0; j < blockSizeBytes; j++)
                {
                    bufXor[j] ^= buffer[i + j];
                }
            }
            
        }
    } while (!feof(f));
    
    if(N < 3){
        *xorRes = (unsigned long long int)(((0xF0 & bufXor[0]) >> 4) ^ (0xF & bufXor[0]));
    }
    else{
        *xorRes = 0;
        for (size_t i = 0; i < blockSizeBytes; i++)
        {
            *xorRes += (unsigned long long int)bufXor[i];
            if(i != blockSizeBytes - 1){
                *xorRes <<= 8;
            }
        }
    }
    free(bufXor);
    free(buffer);
    return SUCCESS;
} 


int areEqualFiles(const char* file1, const char* file2){
    struct stat stat1, stat2;
    stat(file1, &stat1);
    stat(file2, &stat2);
    if (stat1.st_ino == stat2.st_ino && stat1.st_dev == stat2.st_dev) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 3){
        error_handler(NO_ARGUMENTS_ERROR);
        return -1;
    }
    char* commands[] = {"xorN", "mask", "copyN", "find"};
    int ret = 0;
    for (size_t i = 0; i < 4; i++)
    {
        if(!strcmp(commands[i], argv[argc - 2])){
            switch (i)
            {
                case 0:
                    for (size_t j = 1; j < (size_t)argc - 2; j++)
                    {
                        int N;
                        if(!isInt(argv[argc - 1])){
                            error_handler(MUST_BE_INTEGER);
                            return -1;
                        }
                        N = atoi(argv[argc - 1]);
                        if(N < 2 || N > 6){
                            error_handler(WRONG_RANGE);
                            return -1;
                        }

                        unsigned long long int res = 0;

                        FILE* f = fopen(argv[j], "r");
                        if(!f){
                            continue;
                        }
                        ret = xorN(f, N, &res);
                        if(ret == SUCCESS){
                            printf("Mask for file %s is: %0llx in hex and ", argv[j], res);
                            char binary[64];
                            int i = 63;
                            for (; res; res >>= 1){
                                binary[i--] = '0' + (res & 1);
                            } 
                            int ext = (i + 1) % 8;
                            if(ext != 0){
                                while((i + 1) % 8 != 0){
                                    binary[i--] = '0';
                                }
                            }
                            do
                            {
                                putchar(binary[++i]);
                            } while (i != 63);
                            
                            
                            printf(" in bin.\n");
                        }
                        else{
                            continue;
                        }
                        
                    }
                    break;
                case 1:
                    for (size_t j = 1; j < (size_t)argc - 2; j++)
                    {
                        if(!isInt(argv[argc - 1])){
                            error_handler(MUST_BE_INTEGER);
                            return -1;
                        }

                        int res = 0;
                        FILE* f = fopen(argv[j], "r");
                        if(!f){
                            continue;
                        }
                        ret = mask(f, argv[argc - 1], &res);
                        if(ret == SUCCESS){
                            printf("In %s file there are %d integers with mask %s.\n", argv[j], res, argv[argc - 1]);
                        }
                        else{
                            continue;
                        }
                        
                    }
                    break;
                case 2:
                    if(argc > 4){
                        for (size_t i = 0; i < argc - 2; i++)
                        {
                            for (size_t j = i + 1; j < argc - 2; j++)
                            {
                                if(areEqualFiles(argv[i], argv[j])){
                                    error_handler(MUST_NOT_BE_EQUAL_FOR_COPY);
                                    return -1;
                                }
                            }
                            
                        }
                    }
                    copyN(argc, argv);
                    break;
                case 3:
                    find(argc, argv, argv[argc - 1]);
                    break;
            
            default:
                break;
            }
        }
    }
    


    return 0;
}
