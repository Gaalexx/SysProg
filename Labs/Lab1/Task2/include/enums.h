#ifndef ENUMS_H
#define ENUMS_H
    enum ret_t{
        SUCCESS,
        NO_ARGUMENTS_ERROR,
        FILE_ERROR,
        NULL_ERROR,
        MEMORY_ERROR,
        PIPE_ERROR,
        FORK_ERROR,
        MUST_BE_INTEGER,
        MUST_BE_POSITIVE,
        END_OF_FILE,
        WRONG_RANGE,
        MUST_NOT_BE_EQUAL_FOR_COPY,
    };
#endif