#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> /* memset */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../include/sessionDB.h"
#include "../include/message.h"
#include "../include/usrDB.h"

#pragma region CONSTVAR
size_t listener;
fd_set master;
int fdmax;
#pragma endregion

void initServer(char* PORT);
int newConnection();
void monitor(int fdmax, fd_set *restrict read_fds);
void *get_in_addr(struct sockaddr *sa);

int main(int argc, char *argv[]){
    //get port via argv
    listener = atoi(argv[1]);
    initServer(argv[1]);

    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        monitor(fdmax, &read_fds);
    }
}

// return connected FD
int newConnection(){
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];
    //new accept()ed socket descriptor
    int newfd = accept(listener, (struct sockaddr *)& remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    }else{
        FD_SET(newfd, &master); // add to master set
        if (newfd > fdmax) { // keep track of the max
            fdmax = newfd;
        }
        printf("selectserver: new connection from %s on socket %d\n",
               inet_ntop(remoteaddr.ss_family,
                         get_in_addr((struct sockaddr*)&remoteaddr),
                         remoteIP, INET6_ADDRSTRLEN
               ),
               newfd
        );
    }
    return newfd;
}
/*
void joinSession(char*userName, int newSession){
    for(int i=0;i<=curLginUsr;i++){
        if(strcmp(sessionDB[i].usrName, userName)==0){
            sessionDB[i].sessionId=newSession;
            return;
        }
    }
    return;
}

void query(struct message* reply){
    int size=(MAX_NAME+MAX_SESSIONId)*1000+(MAX_SESSIONId*100)+5000;
    char message[size]="User currently online: \n";
    for(int i=0;i<1000;i++){
        if(usrDB[i]!=NULL){
            char name[MAX_NAME];
            char sessionName[MAX_SESSIONId];
            strcpy(name,usrDB[i]->usrName);
            strcpy(sessionName,usrDB[i]->sessionId);
            strcat(message,name);
            strcat(message," ");
            strcat(message,sessionName);
            strcat(message,"\n");
        }
    }
    strcat(message,"Sessions currently avaliable \n")
    for (int i = 0; i < 100; i++)
    {
        if(strlen(sessionDB[i])!=0){
            strcat(message, sessionDB[i]);
            strcat(message, "\n");
        }
    }
    message(reply, size , QUERY, "Admin", message);
    return;
}

void login(struct message* b, struct message* reply){
    char name[MAX_NAME];
    char passwd[MAX_PSSWD];
    strcpy(name,b->source);
    strcpy(passwd,b->data);
    if(not find password and name in usrdatabase){
        message(reply, 27, LO_NAK, "Admin", "username/password not found");
        return;
    }else{
        //createnewusr()
        message(reply, 0, LO_ACK, "Admin", "");
        return;
    }

    //not find
}
*/
void processData(struct message* b, int recvFd){
    //EXIT Case
    printf("processing data\n");
    if(b->type == EXIT){
        close(recvFd);
        FD_CLR(recvFd, &master);
        //clear user in SessionDB if exist
        //removeUser(b->source);
        findUsrInfoByUser("S");
        printf("removed\n");
        return;
    }

    printf("Recv message: ");
    printMessage(b);

    struct message reply;

    enum WEIGHT weight = REGESTER;
//    if(find in the HALL){
//        weight = HALL;
//        printf("inHall\n");
//        //CHECK if in SessionDB and in Session
//    }
//    else if(find in any Session){
//        weight = SESSION;
//        printf("insession\n");
//    }
    switch(weight){
        case REGESTER:
            // Login: check passwd
//            if(b->type == LOGIN){
//                login(b,&reply);
//            }else{
//                message(&reply, 16, CMD_NAK, "Admin",
//                        "Command Not Find, Command Avliable are:\n Login");
//            }
            break;
        case HALL:
            // JOIN SESSION:
            // QUERY:
            // NEW SESSION:
//            if(b->type == NEW_SESS){
//                printf("%i\n",atoi(b->data));
//                createnewSession(atoi(b->data));
//                message(&reply, 0, NS_ACK, "Admin", "");
//            }else{
//                message(&reply, 79, CMD_NAK, "Admin",
//                        "Command Not Find, Command Avliable are:\n joinSession,
//                createSession,logout,list,quit");
//            }
            break;
        case SESSION:
            // EXIT:
            // QUIT:
            // MESSAGE:
            // QUERY:
//            if(b->type == QUERY){
//                //query(&reply);
//            }else{
//                message(&reply, 79, CMD_NAK, "Admin",
//                        "Command Not Find, Command Avliable are:\n joinSession,
//                createSession,leaveSession,logout,list,quit");
//            }
            break;
    }

    // printf("Received message: ");
    // printMessage(b);

    char array[MAX_TOTAL];
    messageToString(array,&reply);
    printf("message Sending: %s\n", array);
    if(send(recvFd,array,strlen(array),0) == -1){
        printf("Send message to Socket: %d failed\n", recvFd);
    }
}


void monitor(int fdmax, fd_set *restrict read_fds){
    // run through the existing connections looking for data to read
    for(int i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, read_fds)){ // we got one!!
            if (i == listener) {
                printf("handle new connections\n");
                // handle new connections
                int recvFd = newConnection();
            }
            else {
                struct message b;
                int errorB = recvMessage(i, &b);
                if(errorB == 0){
                    printf("Client socket %d connection Closed\n", i);
                    close(i); // bye!
                    FD_CLR(i, &master); // remove from master set
                }
                else if(errorB == -1) perror("recv");
                else if(errorB == -2){
                    printf("Invalid Message, client was hacked");
                    close(i); // bye!
                    FD_CLR(i, &master); // remove from master set
                }else{
                    processData(&b,i);
                }
            }
        }
    }
}
void initServer(char* PORT){
    // 1.Socket() 
    // 2.bind()     https://www.cnblogs.com/fnlingnzb-learner/p/7542770.html
    // 3.listen()

    struct addrinfo hints, *ai, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int yes=1; // for setsockopt() SO_REUSEADDR, below
    int rv;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    freeaddrinfo(ai); // all done with this
    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }
    printf("FIN INIT SERVER...\n");
}

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}