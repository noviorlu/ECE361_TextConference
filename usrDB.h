#pragma once

#define MAX_NAME 20
#define MAX_PSSWD 20

struct usrInfo{
    char usrId[MAX_NAME];
    char password[MAX_PSSWD];
};

struct usrInfo usrDB[] = {
    {"jack", "123456"},
    {"bob","101"},
    {"pete","8888"}
};

// return -1 if not found
int findUserInUsrDB(char* userName){
    for(int i=0;i<3;i++){
        if(strcmp(usrDB[i].usrId,userName)==0)
            return i;
    }
    return -1;
}