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

int sender;
int initClient(char *argv[]);
int main(int argc, char *argv[]){
    initClient(argv);
    char buf[100] = "asdqdqfqfqffqwfqfqwfwqfqwffqwfqfqwfqwfqf";
    send(sender, buf, 6, 0);
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