/*
 NAME:

 SYNOPSIS:  To compile program ...
    gcc -o lindorg.buildrooms lindorg.buildrooms.c

 AUTHOR:  Gerson Lindor Jr.
 DATE CREATED: January 26, 2020
 LAST MODIFIED: February 8, 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SIZE 10
#define SELECTED 7
#define NAME_SZ 9
#define CONN_SZ 6
#define MIN 3
#define STR 100

struct room {
    int id;
    char *name;
    int connectCount;
    struct room **connections;
    char *roomType;
};


int makeDir(char * directoryName, int pid);
int writeFile(char *directoryName, struct room **list);
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
void writeOneRoom(FILE *stream,struct room *aRoom);
void createFileName(char *fileName, char *directoryName, struct room *aRoom);
char *getString(char *data);
void createStartAndEnd(struct room **list);



int main() {
    struct room **list = NULL;
    char directoryName[STR] = "lindorg.rooms.";
    int processID = getpid();
    int exitStatus;

    srand(time(NULL));
    /* initialize a list of rooms */
    list = makeRoomList();
    /* generate rooms and make connections */
    makeRandomList(list);
    createGraph(list); 
    createStartAndEnd(list);
    /* create directory */
    exitStatus = makeDir(directoryName, processID);
    if (exitStatus == -1) {
        exit(EXIT_FAILURE);
    }
   /* generate files in the directory */
   if (writeFile(directoryName, list) == 0) {
        exit(EXIT_FAILURE);
   }
   destroyList(list);
   return 0;
}

void createStartAndEnd(struct room **list) {
    char start[15] = "START_ROOM";
    char end[15] = "END_ROOM";
    int head = -1, tail = -1;

    /* create random values for start and end */
    head = rand() % SELECTED;
    do {
        tail = rand() % SELECTED;
    } while (tail == head);
    /* replace old strings with new strings*/
    if (list[head]->roomType) {
        free(list[head]->roomType);
        list[head]->roomType = NULL;
        list[head]->roomType = getString(start);
    }    
    if (list[tail]->roomType) {
        free(list[tail]->roomType);
        list[tail]->roomType = NULL;
        list[tail]->roomType = getString(end);
    }    
}

/*

*/
char *getString(char *data) {
    char *newStr = NULL;
    int size = strlen(data) + 1;

    if (size == 1) { return NULL; }
    newStr = (char *)malloc(size * sizeof(char));
    assert (newStr != 0);
    strcpy(newStr, data);
    return newStr;
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
                /* delete room type */
                free(list[index]->roomType);
                list[index]->roomType = NULL;
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
int makeDir(char * directoryName, int pid) {
    int flag = 0;
    char pidStr[10];

    memset(pidStr, '\0', 10);

    /* change pid to to string */
    flag = snprintf(pidStr, 10, "%d", pid);
    /* concatenate pid string to directory name */
    if (flag > 0) {
        strcat(directoryName, pidStr);
        /* create the directory */
        flag = mkdir(directoryName, 0755);
     } else {
         flag = -1;
     }
    return flag;
}


/*
writes files to a specific directory

*/
int writeFile(char *directoryName, struct room **list) {
    int flag = 1;
    char fileName[STR];
    int i;
    FILE *aFile;

    memset(fileName, '\0', STR);
    /* for each room */
    for ( i = 0; i < SELECTED; ++i) {
        /* create the file name */
        createFileName(fileName, directoryName, list[i]);
        /* open a file */
        aFile = fopen(fileName, "w");
        if (aFile) {
        /* write room contents to a file */
            writeOneRoom(aFile, list[i]);
        } else {
            flag = 0;
            break;
        }
        /* close file */
        fclose(aFile);
        memset(fileName, '\0', STR);
    }
    return flag;
}


/*


*/
void createFileName(char *fileName, char *directoryName, struct room *aRoom) {
    char fowardSlash[2] = "/";
    char suffix[6] = "_room";
    
    /* copy directory name to file name */
    strcpy(fileName, directoryName);
    /* concatenate the forward slash */
    strcat(fileName, fowardSlash);
    /* concatenate the room name */
    strcat(fileName, aRoom->name); 
    /* concatenate the suffix */
    strcat(fileName, suffix); 
}


/*

*/
void writeOneRoom(FILE *stream,struct room *aRoom) {
    int j = 0;

    fprintf(stream, "ROOM NAME: %s\n", aRoom->name);
    while (aRoom->connections[j] && j < aRoom->connectCount) {
        fprintf(stream, "CONNECTION %d: %s\n", (j + 1), aRoom->connections[j]->name);
        ++j;
    }
    fprintf (stream, "ROOM TYPE: %s\n", aRoom->roomType);
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
        list[i]->roomType = (char *)malloc(NAME_SZ * sizeof(char));
        assert(list[i]->roomType != 0);
        strcpy(list[i]->roomType, "MID_ROOM");
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

