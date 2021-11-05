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
 bool sessionOpen[100]={ false };;
int curLginUsr = -1;

void createnewSession(int index){
    if(sessionOpen[index]!=true){
        printf("successfully created session at %d\n",index);
        sessionOpen[index]=true;
        return;
    }
    return;
}



// return -1 if not found
int findUserInSessionDB(char* userName){

    for(int i=0;i<=curLginUsr;i++){
        if(strcmp(sessionDB[i].usrName, userName)==0){
            return i;
        }
    }
    return -1;
}
void removeUser(char* userName){
    printf("removing user\n");
    int index=findUserInSessionDB(userName);
    if(index!=-1){
        for(int i=index;i<=curLginUsr;i++){
            memcpy(sessionDB[i].usrName, sessionDB[i+1].usrName, MAX_NAME);
            sessionDB[i].sessionId=sessionDB[i+1].sessionId;
            sessionDB[i].weight=sessionDB[i+1].weight;
        }
    }
    curLginUsr--;
}

