/*
 NAME:

 SYNOPSIS:  To compile program ...
    gcc -o lindorg.buildrooms lindorg.buildrooms.c

 AUTHOR:  Gerson Lindor Jr.
 DATE CREATED: January 26, 2020
 LAST MODIFIED: ...
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct room {
    char *name;
    struct room **connections;
};


int makeDir(char * directoryName);
int writeFile(char *directoryName, struct room *aRoom);
void createGraph();
int  isGraphful(struct room **list);
void addRandomConnection();
char *roomBank();
struct room *getRandomRoom();
int canAddConnectionFrom(struct room *roomX);
int connectionAlreadyExists(struct room *roomX, struct room *roomY);



int main() {


    return 0;
}



/*
Creates a directory

*/
int makeDir(char * directoryName) {
    int flag = 0;


    return flag;
}


/*
writes files to a specific directory

*/
int writeFile(char *directoryName, struct room *aRoom) {
    int flag = 0;


    return flag;
}


/*
Create all connection in graph

*/
void createGraph() {


}



/*
Return 1 if all rooms have 3 to 6 outbound connections, otherwise returns 0

*/
int  isGraphful(struct room **list) {


}


/*
Adds a random, valid outbound connection from a Room to another Room

*/
void addRandomConnection() {


}


/*

*/
char *roomBank() {
    char *selected = NULL;


    return selected;
}


/*
Returns a random Room, does NOT validate if connection can be added

*/
struct room *getRandomRoom() {
    struct room *roomPtr = (struct room*)malloc(sizeof(struct room));
    assert(roomPtr != 0);

    return  roomPtr;
}


/*
Returns 0 if a connection can be added from Room x ( < 6 outbound connections), otherwise returns 0

*/
int canAddConnectionFrom(struct room *roomX) {
    int flag = 0;

    return flag;
}


/* 
Returns 1 if a connection from Room x to Room y already exists, otherwise returns 0

*/
int connectionAlreadyExists(struct room *roomX, struct room *roomY) {
    int flag = 0;

    return flag;
}


/*
Connect Rooms x and y together, does not check if this connection is valid

*/
void connectionRoom( struct room *roomX, struct room *roomY) {


}


/*
Returns 1 if roomX and roomY are the same room, otherwise returns 0

*/
int isSameRoom(struct room *roomX, struct room *roomY) {
    int flag = 0;

    return flag;
}












