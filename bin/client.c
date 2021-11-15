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

#include "../include/message.h"

#pragma region CONSTVAR
#define STDIN 0
int sender = -1;
fd_set master;
int fdmax;

char usrName[MAX_NAME];
#pragma endregion

int initClient(char* ipAddr, char* port);
int cmdToEnum(char* data);

void processData();
void printData(struct message* b);
void closeConnection();

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
                printf("recving data from server\n");
                struct message b;
                int errorB = recvMessage(sender, &b);
                if(errorB > 0) printData(&b);
                else{
                    if(errorB == 0){
                        printf("Sender socket %d connection Closed\n", sender);
                        closeConnection();
                    }else if(errorB == -1) perror("recv");
                    else if(errorB == -2){
                        printf("Invalid Message, server was hacked");
                        closeConnection();
                    }
                }
            }
        }
    }
    return 0;
}
void new_sess(struct message* b, char* buf){
     int id=0;
     int i=0;
     while(buf[i]!='\0'){
         i++;
     }
     int size=0;
     i++;
     while(buf[i]!='\n'){
         id=(id*10)+buf[i];
         i++;
     }
    char idstr[3];
    sprintf(idstr, "%d", id-'0'); 
    message(b,strlen(idstr),NEW_SESS,usrName,idstr);
    return;
}

void join(struct message* b, char* buf){//这里有问题
    ///createsession\08
    //printf("%s\n",buf);
     int id=0;
     int i=0;
     while(buf[i]!='\0'){
         i++;
     }
     int size=0;
     i++;
     while(buf[i]!='\n'){
         id=(id*10)+buf[i];
         i++;
     }
    char idstr[3];
    sprintf(idstr, "%d", id-'0'); 
    message(b,strlen(idstr),JOIN,usrName,idstr);
    return;
}

// returns -1 if cnnot connect to server
int login(struct message* b, char* buf){
    char* cmd[4];
    for(int i=0; i<4; i++) {
        buf = strtok (NULL, " \n");
        cmd[i] = buf;
    }
    if((initClient(cmd[2], cmd[3])) == -1){
        printf("Cannot connect to server, Please Retry\n");
        return -1;
    }else {
        printf("Connection Successful\n");
        fdmax = sender;
        FD_SET(sender, &master);
    }
    
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
        if(fdmax == STDIN && type != LOGIN){
           printf("LOGIN ERROR: client not logined\n");
           return;
        }
        switch (type){
            case LOGIN:
                if(sender != -1){
                    printf("ProcessData ERROR: client already Logined\n");
                    return;
                } 
                if(login(&b, buf) == -1) return;
                break;
            case EXIT:
                message(&b, 0, EXIT, usrName, "");
                break;
            case QUERY:
                message(&b, 0, QUERY, usrName, "");
                break;
            case NEW_SESS:
                 //message(&b, 1, NEW_SESS, usrName, "1");
                 new_sess(&b,buf);
                break;
            case JOIN:
                 //message(&b, 1, JOIN, usrName, "1");
                 join(&b,buf);
                 break;
            case LEAVE_SESS:
                 message(&b, 0, LEAVE_SESS, usrName, "");
                 break;
        }
    }else{
        //sendMessage
        type = MESSAGE;
    }
    printf("Sending message: ");
    printMessage(&b);

    char str[MAX_TOTAL] = "";
    messageToString(str, &b);
    send(sender, str, strlen(str), 0);
}

void printData(struct message* b){
    //Print recved message from server side
    printf("Recv message: ");
    printMessage(b);
    
    if(b->type == LO_NAK){
        closeConnection();
    }
}

void closeConnection(){
    close(sender); // bye!
    FD_CLR(sender, &master); // remove from master set
    fdmax = STDIN;
    sender = -1;
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

// Convert String into TYPE enum
int cmdToEnum(char* data){
    if(strcmp(data, "/login") == 0) //
        return LOGIN; 
    else if(strcmp(data, "/logout") == 0) 
        return EXIT;
    else if(strcmp(data, "/joinsession") == 0) //
        return JOIN;
    else if(strcmp(data, "/leavesession") == 0) 
        return LEAVE_SESS;
    else if(strcmp(data, "/createsession") == 0) //
        return NEW_SESS;
    else if(strcmp(data, "/list") == 0) //
        return QUERY;
    else if(strcmp(data, "/quit") == 0){
        printf("Quiting Session......Client end.\n");
        exit(0);
    }
    return -1;
}