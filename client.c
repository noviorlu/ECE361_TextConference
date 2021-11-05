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
int sender = -1;
fd_set master;
int fdmax;

char usrName[MAX_NAME];
#pragma endregion

int initClient(char* ipAddr, char* port);
void processData();
void printData(struct message* b);

int main(){
    FD_ZERO(&master);
    FD_SET(STDIN, &master);
    for(;;) {
        // main loop
        fd_set read_fds;
        FD_ZERO(&read_fds);

        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }else{
            if(FD_ISSET(STDIN, &read_fds)){
                processData();
            }else{
                struct message b;
                int errorB = recvMessage(sender, &b);
                if(errorB > 0) printData(&b);
                else{
                    if(errorB == 0){
                        printf("Server socket %d connection Closed\n", sender);
                        close(sender); // bye!
                        FD_CLR(sender, &master); // remove from master set
                    }else if(errorB == -1) perror("recv");
                    else if(errorB == -2){
                        printf("Invalid Message, client was hacked");
                        close(sender); // bye!
                        FD_CLR(sender, &master); //remove from master set
                    }
                }
            }
        }
    }
    return 0;
}

int cmdToEnum(char* data){
    if(strcmp(data, "/login") == 0) 
        return LOGIN; 
    else if(strcmp(data, "/logout") == 0) 
        return EXIT;
    else if(strcmp(data, "/joinsession") == 0) 
        return JOIN;
    else if(strcmp(data, "/leavesession") == 0) 
        return LEAVE_SESS;
    else if(strcmp(data, "/createsession") == 0) 
        return NEW_SESS;
    else if(strcmp(data, "/list") == 0) 
        return QUERY;
    else if(strcmp(data, "/quit") == 0){
        printf("Quiting Session......Client end.\n");
        exit(0);
    }
    return -1;
}
int login(struct message* b, char* buf){
    char* cmd[4];
    for(int i=0; i<4; i++) {
        buf = strtok (NULL, " \n");
        cmd[i] = buf;
    }
    if((initClient(cmd[2], cmd[3])) == -1){
        printf("Cannot connect to server, Please Retry\n");
        return -1;
    }else printf("Connection Successful\n");
    memset(usrName, 0, MAX_NAME);
    strcpy(usrName, cmd[0]);
    message(b, strlen(cmd[1]), LOGIN, usrName, cmd[1]);
    return 0;
}

void processData(){
    char readData[1024] = "";
    read(STDIN, readData, 1024);

    struct message b;
    char* buf;
    buf = strtok(readData, " \n");
    
    int type;

    if(buf[0] == '/'){
        type = cmdToEnum(buf);
        if(type == -1){
            printf("Invalid cmd, please reType\n"); 
            return;
        } 
        switch (type){
            case LOGIN:
                if(login(&b, buf) == -1)return;
                break;
            case EXIT:
                message(&b, 0, EXIT, usrName, "");
            break;
        }
    }else{
        //sendMessage
        type = MESSAGE;
    }
    
    char str[MAX_TOTAL] = "";
    messageToString(str, &b);
    printf("%s\n", str);
    send(sender, str, strlen(str), 0);
}
void printData(struct message* b){
    //Print recved message from server side
    printf("size:%d, ", b->size);
    printf("type:%d, ", b->type);
    printf("source:%s, ", b->source);
    printf("data:%s\n", b->data);
}

int initClient(char* ipAddr, char* port){
    int destPort = atoi(port);
    sender = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(destPort);
    inet_pton(AF_INET, ipAddr, &servAddr.sin_addr);

    return connect(sender, (const struct sockaddr *) &servAddr, sizeof(servAddr));
}