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

#pragma region CONSTVAR
size_t listener;
fd_set master;
int fdmax;
int userSize=0;

#pragma endregion

void initServer(char* PORT);
void newConnection();
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
void newConnection(){
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
}

void login(struct message* b, int sockfd){
    char[MAX_PSSWD] password=b->data;
    char[MAX_NAME] name=b->source;

    for(int i=0;i<3;i++){
        //in userDatabase
        if(usrDB[i].name==name){
            //has correct password
            if(usrDB[i].password==password){
                for(int i=0;i<userSize;i++){
                    //already in session
                    if(sessionDB[i].usrName==name){
                        //NACK, Already logged in
                        sendhelper(socketfd,18,2,"Server","user has logged in")
                    }else{
                        userSize++;
                        if(userSize>=100){
                            //NACK, FULL
                        }else{
                            struct sessionInfo newUser = {name, -1}; 
                            sessionDB[userSize]=newUser;
                            //ACK
                        }
                    }
                }
            }
        }
    }
}
void exit(){
    
}
void dataProcess(struct message* b, int i){
    struct message reply;
    switch(b.type)
    {
        case LOGIN:
            login(b,i);
        break;
        case EXIT:
        ;
        break;
    }
}
void monitor(int fdmax, fd_set *restrict read_fds){
    // run through the existing connections looking for data to read
    for(int i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, read_fds)){ // we got one!!
            if (i == listener) {
                // handle new connections
                newConnection();
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
                    dataProcess(&b,i);
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