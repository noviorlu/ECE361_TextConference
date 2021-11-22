#include "../include/usrDB.h"

const char *filename = "../database/userDB.txt";

void initUserDB(){
    FILE* file = fopen(filename, "r"); /* should check the result */
    char line[100];
    
    char* buf;

    if(file == NULL) printf("not readed\n");
    int idx = 0;
    while (fgets(line, sizeof(line), file)) {
        usrDB[idx] = (UsrInfo*)malloc(sizeof(UsrInfo));
        buf = strtok(line, " \n");
        strcpy(usrDB[idx]->usrId, buf);
        buf = strtok (NULL, " \n");
        strcpy(usrDB[idx]->password, buf);
        idx++;
    }

    fclose(file);
}

// returns -1 if User Already exist
// returns -2 if UserDB are full
int registerUsr(char userName[MAX_NAME], char psswd[MAX_PSSWD]){

    int idx = -1;
    int exist = 0;

    for(int i = 0; i < MAX_USR; i++){
        if(usrDB[i] != NULL){
            if(strcmp(usrDB[i]->usrId, userName) == 0){
                printf("Reg USERDB ERROR: user Exsit\n");
                return -1;
            }
        }else{
            if(idx == -1) idx = i;
        }
        
    }
    if(idx >= MAX_USR) return -2;
    
    usrDB[idx] = (UsrInfo*)malloc(sizeof(UsrInfo));
    strcpy(usrDB[idx]->usrId, userName);
    strcpy(usrDB[idx]->password, psswd);

    FILE* fp = fopen(filename, "a");
    fprintf(fp, "\n%s %s", userName, psswd);
    fclose(fp);
    
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

void printUsrDB(){
    for(int i = 0; i < MAX_USR; i++){
        if(usrDB[i] == NULL)continue;
        printf("Usr: %s Password: %s\n", usrDB[i]->usrId, usrDB[i]->password);
    }
    printf("\n");
}