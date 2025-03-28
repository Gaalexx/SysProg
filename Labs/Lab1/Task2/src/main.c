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
#include "funcs.h"
#include "error_handler.h"

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
                            printf("%s wasn't opened!\n", argv[j]);
                            continue;
                        }
                        ret = mask(f, argv[argc - 1], &res);
                        fclose(f);
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
                        for (size_t i = 0; i < (size_t)argc - 2; i++)
                        {
                            for (size_t j = i + 1; j < (size_t)argc - 2; j++)
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
                    printf("No such command!\n");
                break;
            }
        }
    }
    


    return 0;
}
