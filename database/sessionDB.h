#pragma once

#define MAX_NAME 20
#define MAX_SESSIONId 20
enum WEIGHT{
    REGESTER,
    HALL,
    SESSION
};

typedef struct loginUsrInfo{
    char usrName[MAX_NAME];
    int sockFd;
    char sessionId[MAX_SESSIONId];
    int weight;
}LoginUsrInfo;

typedef struct joinedNode{
    LoginUsrInfo* cur;
    JoinedNode* next;
}JoinedNode;

typedef struct sessionInfo{
    char sessionId[MAX_SESSIONId];
    joinedNode* head;
}SessionInfo;

SessionInfo hall={"HALL", NULL};
SessionInfo* sessionDB[100];

void deleteNode(JoinedNode** head_ref, int key){
    // Store head node
    struct Node *temp = *head_ref, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data == key) {
        *head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory
}



SessionInfo* sessionDB[100];

void createnewSession(int index);
int findUserInSessionDB(char* userName);
void removeUser(char* userName);