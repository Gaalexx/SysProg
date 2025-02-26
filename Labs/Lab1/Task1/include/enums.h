#ifndef ENUMS_H
#define ENUMS_H
enum retType{
    SUCCESS = 0,
    MEMORY_ERROR,
    FILE_ERROR,
    NULL_ERROR,
    NO_SUCH_FUNC,
    ALREADY_LOGINED,
    CONSIST,
    NOT_CONSIST,
    END_OF_FILE,
    ALREADY_EXIST,
    LOGIN_FAILED
};

enum userState{
    LOGINED,
    ADMIN_LOGINED,
    UNLOGINED,
    SANCTIONED
};

#endif