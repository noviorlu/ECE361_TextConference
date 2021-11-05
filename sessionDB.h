#pragma once

#define MAX_NAME 20

enum WEIGHT{
    DEFAULT,
    REGESTER,
    HALL,
    SESSION
};

// struct sessionInfo{
//     char usrName[MAX_NAME];
//     int sessionId;
// };
struct sessionInfo{
    char usrName[MAX_NAME];
    //int sockFd;
    int sessionId;
    int weight;
};

// Initialize a Session Information tuble, include userName, sessionID, weight
void createSessionInfo(struct sessionInfo* newUser, char name[],int id, int weight){
    strcpy(newUser->usrName,name);
    newUser->sessionId=id;
    newUser->weight = weight;
}

struct sessionInfo sessionDB[100]; 
//bool sessionOpen[100];
int curLginUsr = 0;

// return -1 if not found
int findUserInSessionDB(char* userName){
    for(int i=0;i<curLginUsr;i++){
        if(strcmp(sessionDB[i].usrName, userName)==0){
            return i;
        }
    }
    return -1;
}