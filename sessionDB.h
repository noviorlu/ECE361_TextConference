#pragma once

#define MAX_NAME 20

// struct sessionInfo{
//     char usrName[MAX_NAME];
//     int sessionId;
// };
struct sessionInfo{
    char usrName[MAX_NAME];
    int sessionId;
};
struct sessionInfo* sessionDB[100]; 