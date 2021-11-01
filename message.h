#pragma once
#include <sys/socket.h>

#define MAX_DATALEN 4
#define MAX_TYPE 2
#define MAX_NAME 20
#define MAX_DATA 1000
#define MAX_TOTAL MAX_DATALEN + MAX_TYPE + MAX_NAME + MAX_DATA + 3
//MessageType
enum TYPE{
    LOGIN, LO_ACK, LO_NAK,  // 00~02
    EXIT,                   // 03
    JOIN, JN_ACK, JN_NAK,   // 04~06
    LEAVE_SESS,             // 07
    NEW_SESS, NS_ACK,       // 08,09
    MESSAGE,                // 10
    QUERY, QU_ACK           // 11,12
};

struct message {
    unsigned int size;  // actual Length of Data
    unsigned int type;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
};

//Still Working on it, but first Finish the connection 
void recvMessage(int fd, struct message* const destMessage){
    int errorB;
    char buf[MAX_DATALEN + 1] = "";
    while(strlen(buf) < MAX_DATALEN + 1){
        if((errorB = recv(fd, buf, MAX_DATALEN + 1 - strlen(buf), 0)) <= 0){
            // got error or connection closed by client
            if (errorB == 0) {
                // connection closed
                printf("selectserver: client socket %d shutdown\n", fd);
            }else perror("recv");
        }
    }
    if(buf[4]!= ':') printf("Not a message, Dropped\n"); return;
    
    //Read DATALENGTH
    char dataLen[MAX_DATALEN + 1];
    memset(dataLen, '\0', MAX_DATALEN + 1);
    memcpy(dataLen, buf, MAX_DATALEN);
    destMessage->size = atoi(dataLen);
}

//MessageConverter
//EXAMPLE MESSAGE STRING: "DATALENGTH(4):TYPE(2):CLIENTID(8):DATA(9999)"
void stringToMessage(const char * const srcStr, struct message* const destMessage){
    memset(destMessage->source, '\0', MAX_NAME);
    memset(destMessage->data, '\0', MAX_DATA);
    
    int idx = 0;
    
    //Read DATALENGTH
    char dataLen[MAX_DATALEN + 1];
    memset(dataLen, '\0', MAX_DATALEN + 1);
    memcpy(dataLen, srcStr, MAX_DATALEN);
    destMessage->size = atoi(dataLen);
    idx = MAX_DATALEN + 1;

    //Read TYPE
    char type[MAX_TYPE + 1];
    memset(type, '\0', MAX_TYPE + 1);
    memcpy(type, srcStr + idx, MAX_TYPE);
    destMessage->type = atoi(type);
    idx += MAX_TYPE + 1;

    //Read UsrName
    int temp = idx;
    while(srcStr[temp] != ' ') temp++;
    memcpy(destMessage->source, srcStr + idx, temp - idx);
    idx += MAX_NAME + 1;

    //Read Data
    memcpy(destMessage->data, srcStr + idx, destMessage->size);
}

int messageToString(char destStr[MAX_TOTAL], const struct message* const srcMessage){
    memset(destStr, '\0', MAX_TOTAL);

    int strLength = MAX_DATALEN + 1 + MAX_TYPE + 1 + MAX_NAME + 1 + srcMessage->size;
    sprintf(destStr, "%04d:%02d:%-20s:%s", srcMessage->size, srcMessage->type, srcMessage->source, srcMessage->data);
    return strLength;
}

/*int main(){
    struct message a = {11, 32, "jack", "HELLO WORLD"};
    char stra[4+2+20+1000+3];
    memset(stra, '\0', 4+2+20+1000+3);
    int len = messageToString(stra, &a);
    printf("%s   ,   %d\n", stra, len);
    
    struct message b;
    stringToMessage(stra, &b);
    printf("%d, %d, %s, %s\n", b.size, b.type,b.source,b.data);
    return 0;
}*/