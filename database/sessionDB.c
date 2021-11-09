#include "userDB.h"

// Initialize a Session Information tuble, include userName, sessionID, weight
int createUserInfo(char name[], int sockFd){
    int idx = 0;
    while(userDB[idx] != NULL && idx < 1000){idx++};
    if(idx >= 1000){
        printf("SessionDB: User full filled\n");
        return -1;
    }
    userDB[idx] = (UserInfo *)malloc(sizeof(UserInfo));
    strcpy(userDB[idx]->usrName,name);
    memset(userDB[idx]->sessionId, 0, MAX_SESSION);
    userDB[idx]->sockFd = sockFd;
    userDB[idx]->weight = 0;
    return 0;
}

int createnewSession(char sessionName[20]){
    int idx = 0;
    for(int i = 0; i < 100; i++){
        if(strlen(sessionDB[idx]) == 0)idx = i;
        if(strcmp(sessionName, sessionDB[i])==0){
            printf("SessionDB: sessionName already\n");
            return -1;
        }
    }
    strcpy(sessionDB[idx], sessionName);
    return 0;
}

// return -1 if not found
UserInfo* findUserInUserDB(char userName[MAX_NAME]){
    for(int i = 0; i < 1000; i++){
        if(strcmp(userDB[i].usrName, userName)==0){
            return userDB[i];
        }
    }
    printf("SessionDB: user No Found\n");
    return NULL;
}

int removeUser(char userName[MAX_NAME]){
    UserInfo* deleteUsr = findUserInSessionDB(userName);
    if(deleteUsr == NULL){
        printf("SessionDB: user already removed\n");
        return -1;
    }
    free(deleteUsr);
    return 0;
}

// int* broadcastSession(char userName[MAX_NAME]){
//     int *
// }
