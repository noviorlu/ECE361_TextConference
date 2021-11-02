#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "message.h"

#pragma region CONSTVAR
#define STDIN 0
int sender;
fd_set master;
#pragma endregion

int initClient(char *argv[]);
int main(int argc, char *argv[]){
    initClient(argv);

    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // add the sender to the master set
    FD_SET(sender, &master);
    FD_SET(STDIN, &master);

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(sender+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        if(FD_ISSET(STDIN, &read_fds)){
            char buf[1024]="";
            read(STDIN, buf, 1024);
        }else{
            
        }
    }
    // struct message b = {11,23,"jack", "HELLO WORLD"};
    // char str[MAX_TOTAL] = "";
    // messageToString(str, &b);
    // printf("%s\n", str);
    // send(sender, str, strlen(str), 0);
    // for(;;){}
    return 0;
}

int initClient(char *argv[]){
    int destPort = atoi(argv[2]);
    sender = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(destPort);
    inet_pton(AF_INET, argv[1], &servAddr.sin_addr);

    return connect(sender, (const struct sockaddr *) &servAddr, sizeof(servAddr));
}