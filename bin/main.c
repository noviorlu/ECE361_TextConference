#include <stdio.h>
#include "../include/sessionDB.h"
#include "../include/usrDB.h"

int main(){
    printf("Hello world\n");
    initUserDB();
    registerUsr("bob", "3");
    printUsrDB();
    return 0;
}