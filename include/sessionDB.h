#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "var.h"

// General User Info
typedef struct loginUsrInfo{
    char usrName[MAX_NAME];
    int sockFd;
    enum WEIGHT weight;
    int sessionJoined;
}LoginUsrInfo;

// Supportive LinkedList build for
// Joined User in Session
typedef struct joinedNode{
    LoginUsrInfo* user;
    struct joinedNode* next;
}JoinedNode;

// Contains SessionName &
// All user currently joined in this session
typedef struct sessionInfo{
    char sessionId[MAX_SESSIONId];
    JoinedNode* head;
}SessionInfo;

// Hall is SessionDB 0 position
SessionInfo* sessionDB[MAX_SESSION];

extern void initalizeSessionDB();

extern int createSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]);

//extern void deleteSession(char sessionId[MAX_SESSIONId]);

extern int joinSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]);

extern void createUsr(char usrName[MAX_NAME], int sockFd, enum WEIGHT weight);

extern LoginUsrInfo* findUsrInfoByUser(char usrName[MAX_NAME]);
extern SessionInfo* findFirstSessionByUser(char usrName[MAX_NAME]);

extern void leaveAllSession(char usrName[MAX_NAME]);
extern int leaveFromSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]);

int createSession(char sessionId[MAX_SESSIONId]);
SessionInfo* findSession(char sessionId[MAX_SESSIONId]);
void addToSession(LoginUsrInfo* usrInfo, SessionInfo* sessInfo);
LoginUsrInfo* leaveFromSession(char usrName[MAX_NAME], SessionInfo** sessInfo);

extern void printAllUsrInSession(SessionInfo* sessInfo);
extern void printAllSession();