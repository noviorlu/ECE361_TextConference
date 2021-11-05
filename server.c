#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "sessionDB.h"
#include "message.h"
#include "usrDB.h"
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

// void query(struct message* reply){
//     char info[MAX_DATA];
//     for(int i=0;i<=curLginUsr;i++){
//         char sessionInfo[]=sessionDB[i].sessionId+" "+sessionDB[i].usrName+"/n";
//         int size=strlen(sessionInfo);
//         memset(info,sessionInfo,1);
//         info=info+size;
//     }
//     message(reply, ???, QUERY, "Admin", charinfo);
//     return;
// }

void joinSession(struct message* reply,struct sessionInfo* user, int newSession){
    user->sessionId=newSession;
    message(reply, 0, JN_ACK, "Admin", "");
}




void login(struct message* b, struct message* reply){
    for(int i=0;i<3;i++){
        //in userDatabase
        if(strcmp(usrDB[i].password,b->data)==0 && strcmp(usrDB[i].usrId,b->source)==0){
            //has correct password and username
            for(int i=0;i<curLginUsr && strcmp(sessionDB[i].usrName, b->source)==0; i++){
                //already in session & NACK, Already logged in
                message(reply, 17, LO_NAK, "Admin", "Already logged in");
                return;
            }
            curLginUsr++;
            if(curLginUsr>=100){
                //NACK, FULL
                curLginUsr--;
                message(reply, 12, LO_NAK, "Admin", "Server full");
                return;
            }else{
                createSessionInfo(&sessionDB[curLginUsr], b->source,-1, 0);
                //ACK
                message(reply, 0, LO_ACK, "Admin", "");
                return;
            }
        }
    }
    //not find
    message(reply, 27, LO_NAK, "Admin", "username/password not found");
    return;
}
// void exit(){
    
// }
//client -> weight
void processData(struct message* b, int recvFd){
    struct message reply;
    
    enum WEIGHT weight = DEFAULT;
    //Check if the username is in the userDB
    if(findUserInUsrDB(b->source) != -1){
        weight = REGESTER;
        int sessionTuble;
        //CHECK if in the sessionDB (in session or in hall)
        if((sessionTuble = findUserInSessionDB(b->source)) != -1){
            weight = HALL;
            //CHECK if in SessionDB and in Session
            if(sessionDB[sessionTuble].sessionId != -1){
                weight = SESSION;
            }
        }
    }
    
    switch(weight){
        case DEFAULT:
            // NAK USER NOT EXIST
            message(&reply, 14, LO_NAK, "Admin", "User Not Exist");
        break;
        case REGESTER:
            // Login: check passwd
            if(b->type == LOGIN){
                login(b,&reply);
            }
            break;
        case HALL:
            // EXIT: 
            // JOIN SESSION:
                // sessionOpen[sessionNum] ?= true;
            // QUERY:
            // NEW SESSION:
                // sessionOpen[sessionNum] ?= true;
        break;
        case SESSION:
            // EXIT:
            // QUIT:
            // MESSAGE:
            // QUERY:
        break;
    }

    // switch(b->type)
    // {
    //     case LOGIN:
    //         login(b,&reply);
    //     break;
    //     case EXIT:
    //     ;
    //     break;
    //     case MESSAGE:

    //     return;
    // }
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
                    //processData(&b,i);
                    // printf("size:%d, ", b.size);
                    // printf("type:%d, ", b.type);
                    // printf("source:%s, ", b.source);
                    // printf("data:%s\n", b.data);
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