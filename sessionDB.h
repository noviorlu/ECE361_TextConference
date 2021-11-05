#pragma once

#define MAX_NAME 20

// struct sessionInfo{
//     char usrName[MAX_NAME];
//     int sessionId;
// };
struct sessionInfo{
    char usrName[MAX_NAME];
    //int sockFd;
    int sessionId;
};

void createSessionInfo(struct sessionInfo* newUser, char name[],int id){
    strcpy(newUser->usrName,name);
    newUser->sessionId=id;
    return;
}

struct sessionInfo sessionDB[100]; 