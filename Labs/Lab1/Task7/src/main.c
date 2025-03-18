#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <threads.h>
#include <unistd.h>

enum ret_t{
    SUCCESS,
    WRONG_ARGS_NUM,
    DIRECTORY_OPEN_ERROR
};

void error_handler(int ret){
    switch (ret)
    {
    case WRONG_ARGS_NUM:
        printf("Wrong amount of arguments! Must be at least 2!\n");
        break;
    case DIRECTORY_OPEN_ERROR:
        printf("The error occured while oppening the directory!\n");
        break;
    default:
        break;
    }
    return;
}


int listDir(const char* directoryName){
    DIR *dir;
    struct dirent *entry = NULL;

    if(!(dir = opendir(directoryName))){
        return DIRECTORY_OPEN_ERROR;
    }

    while((entry = readdir(dir)) != NULL){
        struct stat* buf = NULL;
        printf("%s\n", entry->d_name);
        int ret = lstat(entry->d_name, buf);
        if(ret == -1){
            perror("lstat");
            continue;
        }
        printf("%d\n", ret);
        
    }
    return 0;
}


int main(int argc, char const *argv[])
{
    
    if(argc == 1){
        char directoryName[] = "./";
        listDir(directoryName);
    }
    else if(argv[argc - 1][0] != '-'){
        for (size_t i = 0; i < (size_t)argc - 1; i++)
        {
            listDir(argv[i]);
        }
        return 0;
    }
    else{
        //по флагу
    }

    return 0;
}
