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

int findUserInUsrDB(char* userName);