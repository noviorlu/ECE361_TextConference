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
<<<<<<< HEAD

void sendhelper(int sender, unsigned int size,unsigned int type,
    unsigned char source[MAX_NAME], unsigned char data[MAX_DATA],){
    struct message b = {size,type,source,data};
    char str[MAX_TOTAL] = "";
    messageToString(str, &b);
    printf("%s\n", str);
    send(sender, str, strlen(str), 0);
}

=======
void message(struct message* b, int size, int type, char* source, char* data){
    b->size = size;
    b->type = type;
    strcpy(b->source, source);
    strcpy(b->data, data);
}
>>>>>>> 74af42a243a30f806453adc0f0bbec05af78b301
int recvPlusSize(int fd, int size, char destStr[size + 1]){
    int errorB = 0;
    for(int i = 0; i < size;){
        if((errorB = recv(fd, destStr + i, size - i, 0)) <= 0)
            return errorB;
        i = strlen(destStr);
    }
    return errorB;
}

int stringToLength(char* lenStr){
    //Read DATALENGTH
    char* buf = strtok(lenStr, ":");
    return atoi(buf);
}

//MessageConverter
void stringToMessage(const char * const srcStr, struct message* const destMessage){
    memset(destMessage->source, '\0', MAX_NAME);
    memset(destMessage->data, '\0', MAX_DATA);
    
    int idx = 0;
    //Read TYPE
    char type[MAX_TYPE + 1];
    memset(type, '\0', MAX_TYPE + 1);
    memcpy(type, srcStr, MAX_TYPE);
    destMessage->type = atoi(type);
    idx += MAX_TYPE + 1;

    //Read UsrName
    int temp = idx;
    while(srcStr[temp] != ' '&&temp<=20+idx) temp++;
    memcpy(destMessage->source, srcStr + idx, temp - idx);
    idx += MAX_NAME + 1;

    // //Read Data
    memcpy(destMessage->data, srcStr + idx, destMessage->size);
}

int messageToString(char destStr[MAX_TOTAL], const struct message* const srcMessage){
    memset(destStr, '\0', MAX_TOTAL);

    int strLength = MAX_DATALEN + 1 + MAX_TYPE + 1 + MAX_NAME + 1 + srcMessage->size;
    sprintf(destStr, "%04d:%02d:%-20s:%s", srcMessage->size, srcMessage->type, srcMessage->source, srcMessage->data);
    return strLength;
}

//EXAMPLE MESSAGE STRING: "TYPE(2):CLIENTID(20):DATA(9999)"
int recvMessage(int fd, struct message* const destMessage){
    char a[MAX_DATALEN+1] = "";
    int errorB;
    errorB = recvPlusSize(fd, MAX_DATALEN+1, a);
    if(errorB <= 0)return errorB;
    if(a[4]!=':') return -2;
    destMessage->size = stringToLength(a);
    
    int extractSize = MAX_TYPE + MAX_NAME + destMessage->size + 2;
    char b[extractSize];
    memset(b, '\0', extractSize);
    errorB = recvPlusSize(fd, extractSize, b);
    if(errorB <= 0)return errorB;
    stringToMessage(b, destMessage);
    return errorB;
}