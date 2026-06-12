#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "ewpdef.h"

int main(int argc, char* argv[]){
    char *id = NULL;
    int port = -1;
    
    for (int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-port") == 0 && i + 1 < argc){
            port = atoi(argv[i + 1 ]);
            i++;
        }else if(strcmp(argv[i], "-id") == 0 && i + 1 < argc){
            id = argv[i + 1]; 
            i++;
        }else{
            fprintf(stderr, "unkown argument: %s\n", argv[i]);
            return 1;
        }
    }
    
    if (port == -1 || id == NULL){
        fprintf(stderr, "usage: %s -port <number> -id <name>\n", argv[0]);
    }
    printf("Logged in as %s\nListening on %d\n", id, port);
    
    int sockFd;
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFd < 0){
        printf("Error opening socket");
    }
    
    struct sockaddr_in saAddr = {0};
    int iPort = port;
    saAddr.sin_family = htons(iPort);
    saAddr.sin_addr.s_addr = htols(0x7F000001);
    if(connect(sockFd, &saAddr, sizeof(saAddr)) < 0){
        printf("Error connecting");
    }
    
    
    struct EWA_EXAM_25_TASK5_PROTOCOL_CLIENTHELLO hello = {0};
    memcpy(hello.stHead.acMagicNumber, "EWP", 3);
    memcpy(hello.acCommand, "HELO", 4);
    memcpy(hello.acFormattedString, id, strlen(id));
    hello.stHead.acDataSize[0] = '0';
    hello.stHead.acDelimiter[0] = '|';
    send(sockFd, &hello, sizeof(hello), 0);
    
    return 0;
}
