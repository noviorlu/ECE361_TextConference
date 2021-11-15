#include "../include/usrDB.h"


void initUserDB(){
    usrDB[0] = (UsrInfo*)malloc(sizeof(UsrInfo));
    usrDB[1] = (UsrInfo*)malloc(sizeof(UsrInfo));
    usrDB[2] = (UsrInfo*)malloc(sizeof(UsrInfo));


    strcpy(usrDB[0]->usrId, "jack");
    strcpy(usrDB[0]->password, "0");
    strcpy(usrDB[1]->usrId, "pete");
    strcpy(usrDB[1]->password, "1");
    strcpy(usrDB[2]->usrId, "alex");
    strcpy(usrDB[2]->password, "2");
}

int findUserInUsrDB(char userName[MAX_NAME], char psswd[MAX_PSSWD]){
   for(int i=0;i<3;i++){
       if(usrDB[i] == NULL) continue;
       if((strcmp(usrDB[i]->usrId,userName) == 0) && 
       (strcmp(usrDB[i]->password,psswd) == 0))
           return i;
   }
   printf("Iterate UserDB ERROR: user not find\n");
    return -1;
}