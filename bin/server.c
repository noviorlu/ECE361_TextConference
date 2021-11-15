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
#include "../include/var.h"

#pragma region CONSTVAR
size_t listener;
fd_set master;
int fdmax;
#pragma endregion

int login(struct message* b, struct message* reply, int recvFd);
void logout(struct message* b, int recvFd);
void join(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply);
void createSess(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply);
void leaveSess(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply);
void query(struct message* reply);

void processData(struct message* b, int recvFd);

void initServer(char* PORT);

void closeConnection(int sockFd);
int newConnection();
void monitor(int fdmax, fd_set *restrict read_fds);
void *get_in_addr(struct sockaddr *sa);

//int main(){
//    initalizeSessionDB();
//    createUsr("Jack", 1);
//    createSession_H("Jack", "a");
//    createSession_H("Jack", "b");
//    createSession_H("Jack", "c");
//    printAllSession();
//    leaveFromSession_H("Jack", "a");
//    printAllSession();
//    leaveAllSession_H("Jack");
//    printAllSession();
//}

int main1(int argc, char *argv[]){
    //get port via argv
    listener = atoi(argv[1]);
    initServer(argv[1]);
    
    initalizeSessionDB();
    initUserDB();

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

void query(struct message* reply){
    // int size=(MAX_NAME+MAX_SESSIONId)*1000+(MAX_SESSIONId*100)+5000;
    char result[MAX_QUERY]="User currently online: \n";

    for(int i = 0; i < MAX_SESSION; i++){
        if(sessionDB[i] == NULL)continue;

        char sessionName[MAX_SESSIONId];
        strcpy(sessionName,sessionDB[i]->sessionId);
        strcat(result,sessionName);
        strcat(result,":\n");

        // printf("%s:\n",sessInfo->sessionId);
        JoinedNode* cur = sessionDB[i]->head;
        while(cur != NULL){
            char name[MAX_NAME];
            strcpy(name,cur->user->usrName);
            //printf("\t%s\n", cur->user->usrName);
            strcat(result,name);
            strcat(result," ");
            cur = cur->next;
        }
        strcat(result,"\n");
    }
    message(reply, strlen(result), QU_ACK, "Admin", result);
    return;
}

void processData(struct message* b, int recvFd){
    //EXIT Case
    if(b->type == EXIT){
        logout(b, recvFd);
        return;
    }

    struct message reply;

    LoginUsrInfo* usr = findUsrInfoByUser(b->source);

    if(b->type == LOGIN){
        if(usr != NULL){
            message(&reply, 18, LO_NAK, "Admin", "usr already Login\n"); 
        }
        else {
            login(b,&reply, recvFd);
        }
    }
    if(usr!=NULL){
        printf("PROCESSING TYPE: %d\n", b->type);
        if(usr->sessionJoined==0){
            printf("inHall\n");
            if(b->type == QUERY){
                query(&reply);
            }else if(b->type == JOIN){
                join(usr->usrName,b->data,&reply);
            }else if(b->type == NEW_SESS){
                createSess(usr->usrName,b->data,&reply);
            }
        }else if(usr->sessionJoined>0){
            printf("inSession\n");
            if(b->type == QUERY){
                query(&reply);
            }else if(b->type == JOIN){
                join(usr->usrName,b->data,&reply);
            }else if(b->type == NEW_SESS){
                createSess(usr->usrName,b->data,&reply);
            }else if(b->type == LEAVE_SESS){
                leaveSess(usr->usrName,b->data,&reply);
            }
        }
    }

//         case SESSION:
//             // EXIT:
//             // QUIT:
//             // MESSAGE:
//             // QUERY:
// //            if(b->type == QUERY){
// //                //query(&reply);
// //            }else{
// //                message(&reply, 79, CMD_NAK, "Admin",
// //                        "Command Not Find, Command Avliable are:\n joinSession,
// //                createSession,leaveSession,logout,list,quit");
// //            }
//             break;
//     }

    char array[MAX_TOTAL];
    messageToString(array,&reply);
    printf("message Sending: %s\n", array);
    if(send(recvFd,array,strlen(array),0) == -1){
        printf("Send message to Socket: %d failed\n", recvFd);
    }
}
void leaveSess(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply){
    printf("LEAVING SESSION\n");
    if(strlen(sessionId) == 0){
        printf("leaving allsession\n");
        leaveAllSession_H(usrName);
        message(reply, 0, LS_ACK, "Admin", "");
        printAllSession();
        return;
    }
    int result = leaveFromSession_H(usrName, sessionId);
    printAllSession();
    if(result==-1){
        message(reply, 41, LS_NAK, "Admin", "leaveSess ERROR: NOT ALLOWED TO LEAVE HALL");
    }else if(result==-2){
        message(reply, 42, LS_NAK, "Admin", "leaveSess ERROR: SESSION NOT EXIST");
    }else if(result==-3){
        message(reply, 41, LS_NAK, "Admin", "leaveSess ERROR: USER NOT EXIST IN SESSION");
    }else{
        message(reply, 0, LS_ACK, "Admin", "");
    }
    return;
}
void createSess(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply){
    int result = createSession_H(usrName,sessionId);
    printAllSession();
    if(result==-1){
        message(reply, 41, NS_NAK, "Admin", "createsession ERROR: JOINED USER NO FOUND");
    }else if(result==-2){
        message(reply, 42, NS_NAK, "Admin", "createsession ERROR: SESSION ALREADY EXIST");
    }else if(result==-3){
        message(reply, 41, NS_NAK, "Admin", "createsession ERROR: SESSIONDB FULLFILLED");
    }else{
        message(reply, 0, NS_ACK, "Admin", "");
    }
    return;
}


void join(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId],struct message* reply){
    int result = joinSession_H(usrName,sessionId);
    printAllSession();
    if(result==-1){
        message(reply, 39, JN_NAK, "Admin", "joinSession ERROR: JOINED USER NO FOUND");
    }else if(result==-2){
        message(reply, 35, JN_NAK, "Admin", "joinSession ERROR: SESSION NO FOUND");
    }else{
        message(reply, 0, JN_ACK, "Admin", "");
    }
    return;
}   

int login(struct message* b, struct message* reply, int recvFd){
    printf("in login\n");
    if(findUserInUsrDB(b->source,b->data) == -1){
        message(reply, 49, LO_NAK, "Admin", "username or password not found, ConnectionClosed\n"); 
        return -1;
    }
    else{
        createUsr(b->source,recvFd);
        printAllSession();
        message(reply, 0, LO_ACK, "Admin", "");
        return 0;
    }
}

void logout(struct message* b, int recvFd){
    close(recvFd);
    FD_CLR(recvFd, &master);
    //clear user in SessionDB if exist
    printf("%s\n", b->source);
    deleteUsr(b->source);
    printAllSession();
    printf("removed\n");
    return;
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
                    closeConnection(i);
                }
                else if(errorB == -1) perror("recv");
                else if(errorB == -2){
                    printf("Invalid Message, client was hacked");
                    closeConnection(i);
                }else{
                    printf("Recv message: ");
                    printMessage(&b);
                    processData(&b,i);
                }
            }
        }
    }
}

void closeConnection(int sockFd){
    close(sockFd); // bye!
    FD_CLR(sockFd, &master); // remove from master set
    LoginUsrInfo* user=findUsrInfoByFd(sockFd);
    if(user!=NULL){
        deleteUsr(user->usrName);
    }
    printAllSession();
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