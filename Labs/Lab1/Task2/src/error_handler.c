#include <stdio.h>
#include <string.h>
#include "enums.h"
#include "error_handler.h"


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
