#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
    
    int error;

    int socketDescr = socket(AF_INET, SOCK_STREAM, 0);
    int clientDescr = 0;
    struct sockaddr_in serverAddr, clientAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5000);

    error = bind(socketDescr, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(struct sockaddr_in));


    error = listen(socketDescr, 10);

    socklen_t addrSize;

    while ((clientDescr = accept(socketDescr, (struct sockaddr*)&clientAddr, &addrSize)) != -1)
    {
        int len = 0;
        if(recv(clientDescr, &len, sizeof(int), 0) == -1) {
            continue;
        }

        char* buffer = (char*)malloc(sizeof(char) * len);
        if(!buffer){
            printf("Error 1");
            return -1;
        }

        if(recv(clientDescr, (void*)buffer, len * sizeof(char), 0) == -1){
            continue;
        }

        
        for (int i = 0; i < len; i++){               //типо какая-то работа
            if(islower(buffer[i])){
                buffer[i] = toupper((int)buffer[i]);
            }
        }

        printf("%s\n", buffer);

        if(send(clientDescr, &len, sizeof(int), 0) == -1){
            continue;
        }

        if(send(clientDescr, (void*)buffer,  len * sizeof(char), 0) == -1){
            continue;
        }
        close(clientDescr);
        free(buffer);
    }
    

    return 0;

}