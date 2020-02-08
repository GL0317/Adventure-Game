/*
 NAME:

 SYNOPSIS:  To compile program ...
    gcc -o lindorg.buildrooms lindorg.buildrooms.c

 AUTHOR:  Gerson Lindor Jr.
 DATE CREATED: January 26, 2020
 LAST MODIFIED: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#define SIZE 10
#define SELECTED 7
#define NAME_SZ 9
#define CONN_SZ 6
#define MIN 3

struct room {
    int id;
    char *name;
    int connectCount;
    struct room **connections;
    char *roomType;
};


int makeDir(char * directoryName);
int writeFile(char *directoryName, struct room *aRoom);
void createGraph(struct room **list);
int  isGraphFull(struct room **list);
void addRandomConnection();
char *roomBank(struct room **list);
int duplicateRooms(struct room **list, char *search);
struct room *getRandomRoom(struct room **list);
int canAddConnectionFrom(struct room *roomX);
int connectionAlreadyExists(struct room *roomX, struct room *roomY);
void destroyList(struct room **list);
int isSameRoom(struct room *roomX, struct room *roomY);
void connectRoom( struct room *roomX, struct room *roomY);
void addRandomConnection(struct room **list);
struct room **makeRoomList();
void destroyList(struct room **list);
void makeRandomList(struct room **list);

/******** Unit Test *******/

void testRooms(struct room **list) {
    int i;
    int j = 0;

    for (i = 0; i < SELECTED; ++i) {
        printf("ROOM NAME: %s\n", list[i]->name);
        while (list[i]->connections[j] && j < CONN_SZ) {
            printf("CONNECTION %d: %s\n", (j + 1), list[i]->connections[j]->name);
            ++j;
        } 
        printf ("Connection Count: %d\n", list[i]->connectCount);
        printf("\n");
        j = 0;
    }
}

void testLoop(char *name, int line) {
    printf("Loop in %s : %d\n", name, line);
}

/*************************/


int main() {
    struct room **list = NULL;

    srand(time(NULL));
    /* initialize a list of rooms */
    list = makeRoomList();
    /* generate rooms and make connections */
    makeRandomList(list);
    createGraph(list); 

    /** run tests **/
/*    testRooms(list); */

    destroyList(list);
    return 0;
}

/* Create a list of room struct pointers */
struct room **makeRoomList() {
    int index;

    struct room **list = (struct room **)malloc(SELECTED * sizeof(struct room*));
    assert(list != 0);
    for (index = 0; index < SELECTED; ++index) {
        list[index] = NULL;
    }
    return list;
}


/* deallocate all memory from the list */
void destroyList(struct room **list) {
    int index;
    int j;
    if (list) {
        for (index = 0; index < SELECTED; ++index) {
            /* go through each index and remove all connections */
            if (list[index] && list[index]->connections) {
                for (j = 0; j < CONN_SZ; ++j) {
                    if (list[index]->connections[j]) {
                        list[index]->connections[j] = NULL;
                    }
                }
            }
            if (list[index]) {
                /* delete name */
                free(list[index]->name);
                list[index]->name = NULL;
                /* reset connection count */
                list[index]->connectCount = 0;
                /* delete room */
                free(list[index]);
                list[index] = NULL;
            }
        }
        /* delete list */
        free(list);
        list = NULL;
    }
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
void createGraph(struct room **list) {
    while (isGraphFull(list) == 0) {
        addRandomConnection(list); 
    }
}



/*
Return 1 if all rooms have 3 to 6 outbound connections, otherwise returns 0

*/
int  isGraphFull(struct room **list) {
    int flag = 1;
    int index = 0;

    assert(list != 0);
    while (index < SELECTED && flag ) {
        if (!list[index]) {
            flag = 0;
        } else if (list[index]->connectCount < MIN){
            flag = 0;
        } 
        ++index;
    }
    return flag;
}


/*
Adds a random, valid outbound connection from a Room to another Room

*/
void addRandomConnection(struct room **list) {
    struct room *A = NULL;
    struct room *B = NULL;

    while (1) {
        /* retrieve room A and see if there can be a connection */ 
        A = getRandomRoom(list);
        if  (canAddConnectionFrom(A)) {
            break;
        }
    }
    do {
        /* retrieve room B */
        B = getRandomRoom(list);
    } while (canAddConnectionFrom(B)== 0 || isSameRoom(A, B) == 1 || connectionAlreadyExists(A, B) == 1);
    connectRoom(A, B);
    connectRoom(B, A);
}


/*

*/
int duplicateRooms(struct room **list, char *search) {
    int i = 0;
    int found = 0;

    while (i < SELECTED && list[i]->name && !found) {
        if (!strcmp(list[i]->name, search)) {
            found = 1;
        }
        ++i;
    }
    return found;
}



/*

*/
char *roomBank(struct room **list) {
    char *selected = NULL;
    int value = -1;
    char *wordBank[10] = { "Gallery", "Ballroom", "Billiard"
                            , "Library", "Office", "Armory"
                            , "Stables", "Chambers", "Kitchen", "Theater" };
    /* generate a random value within the range of wordBank array */
    do { 
        value = rand() % SIZE;
    } while( duplicateRooms(list, wordBank[value])); 
    /* copy random string to selected string */
    selected = (char *) malloc (NAME_SZ * sizeof(char));
    assert(selected != 0);
    strcpy(selected, wordBank[value]);
    return selected;
}


/*

*/
void makeRandomList(struct room **list) {
    int i;
    int j;

    /* loop through each element of list and generate a random room */
    for (i = 0; i < SELECTED; ++i) {
        list[i] = (struct room*)malloc(sizeof(struct room));
        assert(list[i] != 0); 
        list[i]->name = roomBank(list); 
        list[i]->id = i;
        /* allocate the six connections */
        list[i]->connections = (struct room **)malloc(CONN_SZ * sizeof(struct room*));
        assert(list[i]->connections != 0); 
        for (j = 0; j < CONN_SZ; ++j) {
            list[i]->connections[j] = NULL;
        }
        /* initialize connection count and room type */
        list[i]->connectCount = 0;
        list[i]->roomType = NULL;
    }
}


/*
Returns a random Room, does NOT validate if connection can be added

*/
struct room *getRandomRoom(struct room **list) {
    int roomIndex;

    /* get random index for list */
    roomIndex = rand() % SELECTED;
    assert(roomIndex > -1 && roomIndex < SELECTED);
    return  list[roomIndex];
}



/*
Returns 0 if a connection can be added from Room x ( < 6 outbound connections), otherwise returns 0

*/
int canAddConnectionFrom(struct room *roomX) {
    int flag = 0;

    if (roomX->connectCount < CONN_SZ) {
        flag = 1;
     }

    return flag;
}


/* 
Returns 1 if a connection from Room x to Room y already exists, otherwise returns 0

*/
int connectionAlreadyExists(struct room *roomX, struct room *roomY) {
    int flag = 0;
    int i;

    for (i = 0; i < roomX->connectCount; ++i) {
        if (roomX->connections[i]->id == roomY->id) {
            flag = 1;
            break;
        }
    }
    return flag;
}


/*
Connect Rooms x and y together, does not check if this connection is valid

*/
void connectRoom( struct room *roomX, struct room *roomY) {
    int index = roomX->connectCount;
    assert (index < (CONN_SZ + 1));

    if (!roomX->connections[index] && roomX->connectCount < CONN_SZ) {
        roomX->connections[index] = roomY;
        ++roomX->connectCount;
    }
}



/*
Returns 1 if roomX and roomY are the same room, otherwise returns 0

*/
int isSameRoom(struct room *roomX, struct room *roomY) {
    int flag = 0;

    if (roomX->id == roomY->id) {
        flag = 1;
    }
    return flag;
}












