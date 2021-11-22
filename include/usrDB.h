#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "var.h"

typedef struct usrInfo{
    char usrId[MAX_NAME];
    char password[MAX_PSSWD];
}UsrInfo;

UsrInfo* usrDB[MAX_USR];

extern void initUserDB();
extern int registerUsr(char userName[MAX_NAME], char psswd[MAX_PSSWD]);
extern int findUserInUsrDB(char userName[MAX_NAME], char psswd[MAX_PSSWD]);
extern void printUsrDB();