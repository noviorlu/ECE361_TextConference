#include "loginUsrDB.h"

// Initialize a Session Information tuble, include userName, sessionID, weight
int createLoginUsrInfo(char name[MAX_NAME], int sockFd){
    // int idx = 0;
    // while(loginUsrDB[idx] != NULL && idx < 1000){
    //     // If usr already logined
    //     // findUsrIfExist
    //     if(strcmp(loginUsrDB[idx],name)==0){
    //         return 0;
    //     }
    //     idx++;
    // }
    // if(idx >= 1000){
    //     printf("SessionDB: User full filled\n");
    //     return -1;
    // }
    
    // loginUsrDB[idx] = (LoginUsrInfo *)malloc(sizeof(LoginUsrInfo));
    // strcpy(loginUsrDB[idx]->usrName,name);
    // memset(loginUsrDB[idx]->sessionId, 0, MAX_SESSIONId);
    // loginUsrDB[idx]->sockFd = sockFd;
    // loginUsrDB[idx]->weight = 0;
    // return 0;
    if(hall.head=NULL){
        head=(joined)malloc(sizeof(joinedNode));

    }
}

int findSessionIfExist(){

}

// Usr name, find sessionID
// Sessionid find all usrName

LoginUsrInfo** findUsrInSession(char sessionIdp[MAX_SESSIONId]){
    // return a list of Usr that found the same SessionId

}

int deleteSession(char sessionId[MAX_SESSIONId]){
    for(int i=0; i<100; i++){
        if(strcmp(sessionId, sessionDB[i])==0){
            //delete all usrs in the session


            //delete session
            sessionDB[i]=="";
            return;
        }
    }
}
int createNewSession(char sessionName[20]){
    int idx = 0;
    for(int i = 0; i < 100; i++){
        if(strlen(sessionDB[i]) == 0)
            idx = i;
        if(strcmp(sessionName, sessionDB[i])==0){
            printf("SessionDB: sessionName already\n");
            return -1;
        }
    }
    strcpy(sessionDB[idx], sessionName);
    return 0;
}

// return -1 if not found
LoginUsrInfo* findUsrIfExist(char userName[MAX_NAME]){
    for(int i = 0; i < 1000; i++){
        if(strcmp(loginUsrDB[i].usrName, userName)==0){
            return loginUsrDB[i];
        }
    }
    printf("SessionDB: user No Found\n");
    return NULL;
}

int removeUser(char userName[MAX_NAME]){
    LoginUsrInfo* deleteUsr = findUserInSessionDB(userName);
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
