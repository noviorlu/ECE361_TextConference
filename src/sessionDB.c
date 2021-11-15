#include "../include/sessionDB.h"

// #############IMPORTANT###############
//
//  Include initalizeSessionDB before run anything
//
// need to initialize Hall before any user Process
void initalizeSessionDB(){
    if(sessionDB[0] != NULL) free(sessionDB[0]);
    sessionDB[0] = (SessionInfo*)malloc(sizeof(SessionInfo));
    strcpy(sessionDB[0]->sessionId, "HALL");
    sessionDB[0]->head = NULL;
}

// Create session with ID given (dont use findSess because need to mark idx)
// return -1 if user No found
// return -2 if Session Already exist
// return -3 if SessionDB fullfilled
int createSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]){
    
    LoginUsrInfo* usrInfo = findUsrInfoByUser(usrName);
    if(usrInfo == NULL) return -1;
    leaveFromSession(usrName, &sessionDB[0]);
    int errorCode = createSession(sessionId);

    if(errorCode > 0){
        addToSession(usrInfo, sessionDB[errorCode]);
    }else{
        addToSession(usrInfo, sessionDB[0]);
    }
    return errorCode;
}
int createSession(char sessionId[MAX_SESSIONId]){
    bool findEmpty = false;
    int idx = 1;
    for(int i = 1; i < MAX_SESSION; i++){
        // Find an empty spot, also check Duplicated SessionId
        if(sessionDB[i] == NULL){
            if(!findEmpty){
                idx = i;
                findEmpty = true;
            }
        }
        else{
            // Return -1 if Duplicated is find
            if(strcmp(sessionDB[i]->sessionId, sessionId) == 0){
                printf("Create Session ERROR: SESSION \"%s\" ALREADY EXIST\n", sessionId);
                return -2;
            }
        }
    }
    if(findEmpty == false){
        printf("Create Session ERROR: SESSIONDB FULLED\n", sessionId);
        return -3;
    }
    sessionDB[idx] = (SessionInfo*)malloc(sizeof(SessionInfo));
    sessionDB[idx] -> head=NULL;
    strcpy(sessionDB[idx]->sessionId, sessionId);
    return idx;
}

// returns NULL if Session not exist
SessionInfo* findSession(char sessionId[MAX_SESSIONId]){
    for(int i = 0; i < MAX_SESSION; i++){
        if(sessionDB[i] == NULL) continue;
        if(strcmp(sessionDB[i]->sessionId, sessionId) == 0)
            return sessionDB[i];
    }
    return NULL;
}

// move all usr in Session into 'HALL', delete Session
// could not never close 'HALL' - 0 unless server shutdown
//void deleteSession(char sessionId[MAX_SESSIONId]){
//    SessionInfo* sessInfo = findSession(sessionId);
//    JoinedNode* cur = sessionDB[0]->head;
//    while(cur != NULL) cur = cur->next;
//    cur = sessInfo->head;
//    sessInfo->head = NULL;
//    free(sessInfo);
//    sessInfo = NULL;
//}

// join a session for a user,
// if sessionJoined is 0, remove from HALL and sessionJoined + 1
// else only sessionJoined + 1
// return -1 if Join User no Found
// return -2 if Session no FOund
int joinSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]){
    LoginUsrInfo* usrInfo = findUsrInfoByUser(usrName);
    if(usrInfo == NULL) {
        printf("joinSession ERROR: JOINED USER NO FOUND\n");
        return -1;
    }

    SessionInfo* sessInfo = findSession(sessionId);
    if(sessInfo == NULL){
        printf("joinSession ERROR: SESSION NO FOUND\n");
        return -2;
    }

    addToSession(usrInfo, sessInfo);
    return 0;
}

// add User to session and control sessionJoined Number
void addToSession(LoginUsrInfo* usrInfo, SessionInfo* sessInfo){
    if(sessInfo != sessionDB[0])
        usrInfo->sessionJoined++;
    else usrInfo->sessionJoined = 0;

    JoinedNode* insertNode = (JoinedNode*) malloc(sizeof(JoinedNode));
    insertNode->user = usrInfo;
    insertNode->next = NULL;

    JoinedNode* cur = sessInfo->head;
    if(cur == NULL) sessInfo->head = insertNode;
    else{
        while(cur->next != NULL) cur = cur->next;
        cur->next = insertNode;
    }
}


// remove user from all Sessions and join 'HALL'
// return early if User already in NULL
LoginUsrInfo* leaveAllSession(char usrName[MAX_NAME]){
    LoginUsrInfo* usrInfo;
    for(int i = 0; i < MAX_SESSION && sessionDB[i] != NULL; i++){
        LoginUsrInfo* temp = leaveFromSession(usrName, &sessionDB[i]);
        if(temp != NULL) usrInfo = temp;
    }
    if(usrInfo == NULL) {
        printf("LeaveAllSession: user already leaved all Session");
        return NULL;
    }
    return usrInfo;
}

void deleteUsr(char usrName[MAX_NAME]){
    if(strlen(usrName)==0) return;
    LoginUsrInfo* usrInfo = leaveAllSession(usrName);
    if(usrInfo != NULL) free(usrInfo);
}

void leaveAllSession_H(char usrName[MAX_NAME]){
    LoginUsrInfo* usrInfo = leaveAllSession(usrName);
    addToSession(usrInfo, sessionDB[0]);
}

// remove a certain userInfo in an specific Session
// returns -1 if trying to leave from HALL
// returns -2 if Session Not exist
// returns -3 if User Not eixst in Session
int leaveFromSession_H(char usrName[MAX_NAME], char sessionId[MAX_SESSIONId]){
    if(strcmp(sessionId, "HALL") == 0){
        printf("LEAVE Session ERROR: cannot leave from Hall\n");
        return -1;
    }
    SessionInfo* sessInfo = findSession(sessionId);
    if(sessInfo == NULL) return -2;
    LoginUsrInfo* usrInfo = leaveFromSession(usrName, &sessInfo);
    if(usrInfo == NULL) return -3;
    if(usrInfo->sessionJoined == 0)
        addToSession(usrInfo, sessionDB[0]);
    return 0;
}

// returns the UserInfo for further operation
// controls the number of sessionJoined
LoginUsrInfo* leaveFromSession(char usrName[MAX_NAME], SessionInfo** sessInfo){
    JoinedNode* cur = (*sessInfo)->head, *prev;
    if(cur != NULL && strcmp(cur->user->usrName, usrName) == 0){
        (*sessInfo)->head = cur->next;
        goto ESCAPE;
    }
    while (cur != NULL && strcmp(cur->user->usrName, usrName) != 0) {
        prev = cur;
        cur = cur->next;
    }

    // If key was not present in linked list
    if (cur == NULL) return NULL;

    // Unlink the node from linked list
    prev->next = cur->next;

ESCAPE:
    cur->user->sessionJoined--;

    if((*sessInfo)->head == NULL && (*sessInfo) != sessionDB[0]){
        printf("SESSION \"%s\" Delete\n", (*sessInfo)->sessionId);
        free(*sessInfo);
        (*sessInfo) = NULL;
    }

    LoginUsrInfo* usrInfo = cur->user;
    free(cur);
    return usrInfo;
}

// allocate New user and place into 'HALL'
void createUsr(char usrName[MAX_NAME], int sockFd){
    LoginUsrInfo* temp = (LoginUsrInfo*)malloc(sizeof(LoginUsrInfo));
    strcpy(temp->usrName, usrName);
    temp->sockFd = sockFd;
    temp->sessionJoined = 0;

    addToSession(temp, sessionDB[0]);
}

// returns NULL if User not exist
LoginUsrInfo* findUsrInfoByFd(int sockFd){
    for(int i = 0; i < MAX_SESSION; i++){
        if(sessionDB[i] == NULL) continue;
        JoinedNode* cur = sessionDB[i]->head;
        while(cur != NULL){
            if(cur->user != NULL && cur->user->sockFd == sockFd)
                return cur->user;
            cur = cur->next;
        }
    }
    return NULL;
}

// returns NULL if User not exist
LoginUsrInfo* findUsrInfoByUser(char usrName[MAX_NAME]){
    for(int i = 0; i < MAX_SESSION; i++){
        if(sessionDB[i] == NULL) continue;
        JoinedNode* cur = sessionDB[i]->head;
        while(cur != NULL){
            if(cur->user != NULL && strcmp(cur->user->usrName, usrName) == 0)
                return cur->user;
            cur = cur->next;
        }
    }
    return NULL;
}

// Given userName find the first SessionId that contains userName
// record the checking process by int ptr range(0, MAX_SESSION)
// return NULL if found no more (return the first founded Session)
SessionInfo* findFirstSessionByUser(char usrName[MAX_NAME]){
    for(int i = 0; i < MAX_SESSION; i++){
        JoinedNode* cur = sessionDB[i]->head;
        while(cur != NULL) {
            if(cur->user != NULL && strcmp(cur->user->usrName, usrName) == 0){
                return sessionDB[i];
            }
            cur = cur->next;
        }
    }
    return NULL;
}

// Testing Useage
void printAllUsrInSession(SessionInfo* sessInfo){
    if(sessInfo == NULL) return;
    printf("%s:\n",sessInfo->sessionId);
    JoinedNode* cur = sessInfo->head;
    while(cur != NULL){
        printf("\t%s\n", cur->user->usrName);
        cur = cur->next;
    }
}
void printAllSession(){
    for(int i = 0; i < MAX_SESSION; i++){
        if(sessionDB[i] == NULL)continue;
        printAllUsrInSession(sessionDB[i]);
    }
    printf("\n");
}