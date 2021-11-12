#pragma once

#define MAX_NAME 20
#define MAX_SESSIONId 20
enum WEIGHT{
    DEFAULT,
    REGESTER,
    HALL,
    SESSION
};

typedef struct userInfo{
    char usrName[MAX_NAME];
    int sockFd;
    char sessionId[MAX_SESSIONId];
    int weight;
}UserInfo;

UserInfo* userDB[1000];

char sessionDB[100][MAX_SESSIONId] = {""};

void createnewSession(int index);
int findUserInSessionDB(char* userName);
void removeUser(char* userName);