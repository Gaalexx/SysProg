#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <pthread.h>

#define MESSAGE_SIZE 1024
#define STRINGIZE(x) #x
#define MS_STR(x) STRINGIZE(x)
#define structSiz(strct) (sizeof(strct.messageText) + sizeof(strct.from) + sizeof(strct.userName)) 


struct message
{
    long mtype;
    char messageText[MESSAGE_SIZE];
    int from;
    char userName[128];
};

enum messageTypes{
    TO_SERVER = 1,
    TO_CLIENT = 2
};


static int msgId;
static pid_t pid;

void* messageRcv(void* arg){
    (void)arg;
    while (1)
    {
        struct message reply;
        if (msgrcv(msgId, &reply, structSiz(reply), pid, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Server answer: %s", reply.messageText);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    key_t key = ftok("/home/gaalex/Programs/SysProg/SysProg/Labs/Lab1/Task4/Server/output/main", 65);

    pid = getpid();
        
    if(key == -1){
        printf("Ftok error!\n");
        return -1;
    }
    msgId = msgget(key, 0700);
    if(msgId == -1){
        printf("Message error!\n");
        return -1;
    }

    pthread_t rcvThread;
    if(pthread_create(&rcvThread, NULL, messageRcv, NULL) == -1){
        printf("Thread error\n");
        return -1;
    }


    char username[128];
    printf("Input your name: ");
    scanf("%128s", username);
    {
        struct message reg;
        reg.mtype = TO_SERVER;
        reg.from = (int)pid;
        strcpy(reg.messageText,"registration");
        strcpy(reg.userName, username);
        if (msgsnd(msgId, &reg, structSiz(reg), 0) == -1) {
            perror("msgsnd");
            return -1;
        }
    }

    while(1){
        char buf[MESSAGE_SIZE];
        struct message msg;
        msg.mtype = TO_SERVER;
        msg.from = (int)pid;
        char c;
        int ii = 0;
        fflush(NULL);
        while((c = getchar()) != '\n'){
            buf[ii++] = c;
            buf[ii] = '\0';
        }

        if(!strcmp(buf, "exit")){
            printf("Exit\n");
            pthread_cancel(rcvThread);
            return 0;
        }
        strcpy(msg.messageText, buf);
        strcpy(msg.userName, username);

        if (msgsnd(msgId, &msg, structSiz(msg), 0) == -1) {
            perror("msgsnd");
            pthread_cancel(rcvThread);
            return -1;
        }

        

    }
    pthread_cancel(rcvThread);
    return 0;
}
