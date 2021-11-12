#include <stdio.h>
#include "../include/sessionDB.h"
#include "../include/usrDB.h"

int main(){
    initalizeSessionDB();
    createUsr("Jack", 3, REGESTER);
    createUsr("Bob", 4, REGESTER);
    createUsr("Pete", 5, REGESTER);
    createUsr("Alex", 6, REGESTER);
    printf("Pete's sock: %d\n", findUsrInfoByUser("Pete")->sockFd);
    printAllSession();

    createSession_H("Pete", "YESYES");
    printAllSession();
    createSession_H("Pete", "NONO");
    printAllSession();
    leaveAllSession("Pete");
    printAllSession();
    createSession_H("Pete", "NONO");
    printAllSession();
    createSession_H("Jack", "NONO");
    printAllSession();
    createSession_H("Jack", "YESYES");
    joinSession_H("Jack", "NONO");
    joinSession_H("Alex", "NONO");
    joinSession_H("Bob", "NONO");
    printAllSession();
    leaveFromSession_H("Jack", "NONO");
    printAllSession();
    return 0;
}