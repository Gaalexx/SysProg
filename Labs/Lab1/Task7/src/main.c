#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <threads.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>

#define BLUE    "\x1B[34m"  
#define CYAN    "\x1B[36m"

enum ret_t{
    SUCCESS,
    MEMORY_ERROR,
    WRONG_ARGS_NUM,
    DIRECTORY_OPEN_ERROR, 
    NULL_ERROR,
    PATH_ERROR
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
    case MEMORY_ERROR:
        printf("Memory error occured!\n");
        break;
    case NULL_ERROR:
        printf("Null error!\n");
        break;
    case PATH_ERROR:
        printf("The error with path occured!\n");
        break;
    default:
        break;
    }
    return;
}

int absdir(const char* relativePath, char* newPath) {
    if(realpath(relativePath, newPath)){
        short len = (short)strlen(newPath);
        newPath[len++] = '/';
        newPath[len] = '\0';
        return SUCCESS;
    }else{
        return PATH_ERROR;
    }
}

int dynConcat(char** result, const char* s1, const char* s2){
    size_t realLen = strlen(s1) + strlen(s2) + 1;
    (*result) = (char*)malloc(realLen * sizeof(char));
    if(!(*result)){
        return MEMORY_ERROR;
    }
    strcpy((*result), s1);
    strcat((*result), s2);
    (*result)[realLen - 1] = '\0';
    return SUCCESS;
}

void fileRightsPrint(mode_t mode) {
    if(S_ISLNK(mode)){
        printf("l");
    }
    else if(S_ISDIR(mode)){
        printf("d");
    }
    else{
        printf("-");
    }
    printf((mode & S_IRUSR) ? "r" : "-"); 
    printf((mode & S_IWUSR) ? "w" : "-"); 
    printf((mode & S_IXUSR) ? "x" : "-"); 
    printf((mode & S_IRGRP) ? "r" : "-"); 
    printf((mode & S_IWGRP) ? "w" : "-"); 
    printf((mode & S_IXGRP) ? "x" : "-"); 
    printf((mode & S_IROTH) ? "r" : "-"); 
    printf((mode & S_IWOTH) ? "w" : "-"); 
    printf((mode & S_IXOTH) ? "x" : "-"); 
}

void userNamePrint(struct stat st){
    struct passwd *pw = getpwuid(st.st_uid);
    if(pw == NULL){
        printf("none");
    }
    printf("%s", pw->pw_name);
    return;
}

void dirNamePrint(struct stat st){
    struct passwd *pw = getpwuid(st.st_uid);
    if(pw == NULL){
        printf("none");
    }
    printf("%s", pw->pw_dir);
    return;
}

void datePrint(struct stat st) {
    char buffer[1024];
    time_t timestamp = st.st_mtime;
    struct tm *tm_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%m/%d/%H:%M", tm_info);
    printf("%s", buffer);
}


int listDir(const char* directoryName, const char* flag){
    DIR *dir;
    struct dirent *entry = NULL;
    if(flag == NULL){
        return NULL_ERROR;
    }

    if(!(dir = opendir(directoryName))){
        return DIRECTORY_OPEN_ERROR;
    }
    
    while((entry = readdir(dir)) != NULL){
        struct stat buf;

        char* fullPath;
        if(dynConcat(&fullPath, directoryName, entry->d_name) != SUCCESS){
            closedir(dir);
            return MEMORY_ERROR;
        }
        

        int ret = lstat(fullPath, &buf);
        if(ret == -1){
            free(fullPath);
            closedir(dir);
            return DIRECTORY_OPEN_ERROR;
        }
        if(!strcmp(flag, "-la") || !strcmp(flag, "-al")){
            fileRightsPrint(buf.st_mode);
            putchar('\t');
            printf("%ld ", buf.st_nlink);
            putchar('\t');
            userNamePrint(buf);
            putchar('\t');
            dirNamePrint(buf);
            putchar('\t');
            printf("%ld", buf.st_size);
            putchar('\t');
            datePrint(buf);
            putchar('\t');
            if(S_ISDIR(buf.st_mode)){
                printf(BLUE "%s\x1b[0m\n", entry->d_name);
            }
            else if(S_ISLNK(buf.st_mode)){
                printf(CYAN "%s\x1b[0m\n", entry->d_name);
            }
            else{
                printf("%s\n", entry->d_name);
            }
            
        }else{
                switch (flag[1])
            {
            case ' ':
                if(entry->d_name[0] != '.'){ 
                    if(S_ISDIR(buf.st_mode)){
                        printf("%20ld\t"BLUE "%s\x1b[0m\n", entry->d_ino, entry->d_name);
                    }
                    else if(S_ISLNK(buf.st_mode)){
                        printf("%20ld\t"CYAN "%s\x1b[0m\n", entry->d_ino, entry->d_name);
                    }
                    else{
                        printf("%20ld\t%s\n", entry->d_ino, entry->d_name);
                    }
                }
                break;
            case 'a':
                if(S_ISDIR(buf.st_mode)){
                    printf("%20ld\t"BLUE "%s\x1b[0m\n", entry->d_ino, entry->d_name);
                }
                else if(S_ISLNK(buf.st_mode)){
                    printf("%20ld\t"CYAN "%s\x1b[0m\n", entry->d_ino, entry->d_name);
                }
                else{
                    printf("%20ld\t%s\n", entry->d_ino, entry->d_name);
                }
                break;
            case 'l':
            if(entry->d_name[0] != '.'){
                fileRightsPrint(buf.st_mode);
                putchar('\t');
                printf("%ld ", buf.st_nlink);
                putchar('\t');
                userNamePrint(buf);
                putchar('\t');
                dirNamePrint(buf);
                putchar('\t');
                printf("%ld", buf.st_size);
                putchar('\t');
                datePrint(buf);
                putchar('\t');
                if(S_ISDIR(buf.st_mode)){
                    printf(BLUE "%s\x1b[0m\n", entry->d_name);
                }
                else if(S_ISLNK(buf.st_mode)){
                    printf(CYAN "%s\x1b[0m\n", entry->d_name);
                }
                else{
                    printf("%s\n", entry->d_name);
                }
                }   
                break;
            default:
                printf("Unknown key!\n");
                break;
       }
        }
        free(fullPath);
    }
    closedir(dir);
    return SUCCESS;
}


int main(int argc, char const *argv[])
{
    int ret;
    char nPath[PATH_MAX];
    if(argc == 1){
        char buf[4096];
        strcpy(buf, argv[0]);
        char* ptr = strrchr(buf, '/');
        if(!ptr){
            error_handler(NULL_ERROR);
            return -1;
        }
        *(ptr+1) = '\0';
        printf("For %s directory:\n", buf);
        ret = listDir(buf, "  ");
    }
    else if(argv[argc - 1][0] != '-'){
        for (size_t i = 1; i < (size_t)argc; i++)
        {
            absdir(argv[i], nPath);
            printf("For %s directory:\n", nPath);
            ret = listDir(nPath, "  ");
        }
    }
    else{
        for (size_t i = 1; i < (size_t)argc - 1; i++)
        {   
            absdir(argv[i], nPath);
            printf("For %s directory:\n", nPath);
            ret = listDir(nPath, argv[argc - 1]);
        }
    }
    error_handler(ret);
    return 0;
}
