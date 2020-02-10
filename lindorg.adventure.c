/*
 NAME:

 SYNOPSIS:  To compile program ...
    gcc -o lindorg.adventure lindorg.adventure.c -lpthread

 AUTHOR:  Gerson Lindor Jr. (lindorg@oregonstate.edu)
 DATE CREATED: February 8, 2020
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
#include <dirent.h>
#include <pthread.h>


#define SELECTED 7
#define NAME_SZ 9
#define CONN_SZ 6
#define STR 200


struct room {
    char *name;
    int connectCount;
    char **connections;
    char *roomType;
};



struct room **makeRoomList();
void destroyList(struct room **list);
void createFileName(char *fileName, char *directoryName, struct room *aRoom);
char *getString(char *data);
char *getData(char *line);
void readOneRoom(FILE *reader,struct room *aRoom, char *line);
char *openDirectory();
void readDirectory (char *directoryName, struct room **list);
void readFile(char *directoryName, char *fileName, struct room *aRoom);
void countAllConnections (struct room **list);
void displayRoom(FILE *stream,struct room *aRoom);
int searchRooms(struct room **list, char *item, int section);
int searchConnections(struct room *aRoom, char *item);
void mainMenu(struct room *aRoom);
int checkInput(struct room **list, struct room *aRoom, char *response);
int prompt(struct room **list, int index, int showMenu);
void *getTime(void *argument);

/*** tests ***/
/*

*/
void writeRoom(FILE *stream,struct room *aRoom) {
    int j = 0;

    fprintf(stream, "ROOM NAME: %s\n", aRoom->name);
    while (aRoom->connections[j] && j < CONN_SZ) {
        fprintf(stream, "CONNECTION %d: %s\n", (j + 1), aRoom->connections[j]);
        ++j;
    }
    fprintf (stream, "ROOM TYPE: %s\n", aRoom->roomType);
    fprintf (stream, "COUNT: %d\n", aRoom->connectCount);
}

void showAll(struct room **list) {
    int i;

    for (i = 0; i < SELECTED; ++i) {
        displayRoom(stdout, list[i]);
    }
}

/***************/
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;


int main() {
    char *directoryName = NULL;
    struct room **list = NULL;
    int start = -1, end = -1, result = -1;
    char *victoryPath[1000];
    int vStep = 0, previousRoom = -1;
    int resultCode;
    pthread_t myThreadID;


    /* set up the game */
    list = makeRoomList();
    directoryName = openDirectory();
    readDirectory(directoryName, list);
    printf("----Bank ----\n");
 /*   showAll(list);                 */
    printf("------------\n");
    /* interact with user */
    /* main mutex lock*/
    pthread_mutex_lock(&myMutex);
    /* create second thread */
    resultCode = pthread_create(&myThreadID, NULL, getTime, NULL);
    /* find start room */
    start = searchRooms(list, "N/A", 0);
    /* find end room */
    end = searchRooms(list, "N/A", 1);

    printf("End Room = %d\n", end);
    do {
    /* prompt user */
        previousRoom = start;
        result = prompt(list, start, 1);
        /* when time is called, stay in the same room */
        if (result == SELECTED) {
             /* main mutex unlock */
            pthread_mutex_unlock(&myMutex);
             /* use pthread join to block until second thread completes */
             resultCode = pthread_join(myThreadID, NULL);
            /* main mutex lock */
            pthread_mutex_lock(&myMutex);
            /* recreate the second thread */
            resultCode = pthread_create( &myThreadID, NULL, getTime, NULL);
            result = previousRoom; 
            result = prompt(list, result, 0);
        }
        start = result;
        if (result != previousRoom) { 
            victoryPath[vStep] = list[result]->name;
            ++vStep;
        }
    } while (start != end);
    /* victory message */
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", vStep);
    int i;
    for(i = 0; i < vStep; ++i) {
        printf("%s\n", victoryPath[i]);
    }
    if (directoryName) { free(directoryName); directoryName = NULL; }
    destroyList(list);
    return 0;
}


void *getTime(void *argument) {
    /* mutext lock in the 2nd thread */
    pthread_mutex_lock(&myMutex);
    printf("\n1:03pm, Tuesday, September 13, 2016\n\n");
/*    printf("WHERE TO? >)");  */
    pthread_mutex_unlock(&myMutex);
    /* unlock the mutext */
}


int prompt(struct room **list, int index, int showMenu) {
    char response[STR];
    int i, strSize, before;

    do {
        before = index;
        memset(response, '\0', STR);
        if (showMenu) {
            mainMenu(list[index]);
        }
        printf("WHERE TO? >");
        fgets(response, STR, stdin);
        strSize = strlen(response);
        /* trim off new line */
        for (i = strSize; i > 0; --i) {
            if (response[i] == '\n') {
                response[i] = '\0';
                break;
            }
        }
        index = checkInput(list, list[index], response);
        if (index == -1) {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            index = before;
        }
    } while (index == -1);
    return index;
}


/*

*/
void mainMenu(struct room *aRoom) {
    int j = 0;

    fprintf(stdout, "CURRENT LOCATIONS: %s\n", aRoom->name);
    printf("POSSIBLE CONNECTIONS: ");
    while (aRoom->connections[j] && j < (aRoom->connectCount - 1)) {
        fprintf(stdout, "%s,", aRoom->connections[j]);
        ++j;
    }
    fprintf(stdout, "%s.\n", aRoom->connections[j]);
    fprintf (stdout, "ROOM TYPE: %s\n", aRoom->roomType);
}


int checkInput(struct room **list, struct room *aRoom, char *response) {
    int roomIndex = -1;
    int size = strlen(response) + 1;

    if (size == 0 || size > 9) {
        return -1;
    }
    if(strcmp(response, "time") != 0) {
        roomIndex = searchConnections(aRoom, response);
        /* search for the room */
        if (roomIndex != -1) {
            roomIndex = searchRooms(list, aRoom->connections[roomIndex], 2);
        }
     } else { 
         return SELECTED;
     }
     return roomIndex;
}


/*

*/
int searchConnections(struct room *aRoom, char *item) {
    int i, found = -1;

    for (i = 0; i < aRoom->connectCount; ++i) {
        if (!strcmp(aRoom->connections[i], item)) {
            found = i;
            break;
        }
    }
    return found;
}


/*

*/
int searchRooms(struct room **list, char *item, int section) {
    int startRoom = -1;
    int endRoom = -1;
    int foundRoom = -1;
    int result, i;
    
    for (i = 0; i < SELECTED; ++i) {
        if ((strcmp(list[i]->name, item) == 0) && section == 2) {
            foundRoom = i;
            break;
         }
        if ((strcmp(list[i]->roomType, "START_ROOM") == 0) && section == 0) {
            startRoom = i;
        }
        if ((strcmp(list[i]->roomType, "END_ROOM") == 0) && section == 1) {
            endRoom = i;
        }
    }
    switch(section) {
        case 0:
            result = startRoom;
            break;
        case 1:
            result = endRoom;
            break;
        case 2:
            result = foundRoom;
            break;
    }
    return result;
}


/*

*/
void displayRoom(FILE *stream,struct room *aRoom) {
    int j = 0;

    fprintf(stream, "ROOM NAME: %s\n", aRoom->name);
    while (aRoom->connections[j] && j < CONN_SZ) {
        fprintf(stream, "CONNECTION %d: %s\n", (j + 1), aRoom->connections[j]);
        ++j;
    }
    fprintf (stream, "ROOM TYPE: %s\n", aRoom->roomType);
    printf("\n");
}


/*

*/
void countAllConnections (struct room **list) {
    int i, j, count;

    for (i = 0; i < SELECTED; ++i) {
        count = 0;
        j = 0;
        while (list[i]->connections[j] && j < CONN_SZ) {
            ++count;
            ++j;
        }
        list[i]->connectCount = count;
    }
}


/*

*/
char *openDirectory() {
    int newestTime = -1;
    char *newDirName = NULL;
    char target[20] = "lindorg.rooms.";
    DIR *dirToCheck;
    struct dirent *fileInDir;
    struct stat dirAttributes;

    /* open current directory */
    dirToCheck = opendir(".");
    if (dirToCheck > 0) { /* check if its open */
        /* search each entry in the directory */
        while ((fileInDir = readdir(dirToCheck)) != NULL) {
            /* if prefix match entry */
            if (strstr(fileInDir->d_name, target) != NULL) {
                /* get the attributes of the entry */
                stat(fileInDir->d_name, &dirAttributes);
                /* get the most recent directory name */
                if ((int)dirAttributes.st_mtime > newestTime) {
                    newestTime = (int)dirAttributes.st_mtime;
                    if (newDirName) { free(newDirName); newDirName = NULL; }
                    newDirName = (char *)malloc(STR * sizeof(char));
                    assert(newDirName != 0);
                    strcpy(newDirName, fileInDir->d_name);
                }
            }
        }
    }
    /* close directory */
    closedir(dirToCheck);
    return newDirName;
}


/*

*/
void readDirectory (char *directoryName, struct room **list) {
    DIR *dirToCheck;
    char *target = "_room";
    int roomIndex = 0;
    struct dirent *fileInDir;
    char fileName[STR];

    memset(fileName, '\0', STR);
    /* open specified directory */
    dirToCheck = opendir(directoryName);
    if (dirToCheck > 0) { /* check if its open */
        /* search each entry in the directory */
        while ((fileInDir = readdir(dirToCheck)) != NULL) {
            /* if prefix match entry */
            if (strstr(fileInDir->d_name, target) != NULL) {
                memset(fileName, '\0', STR);
                strcpy(fileName, fileInDir->d_name);
                /* read the file */
                readFile(directoryName, fileName, list[roomIndex]);
                ++roomIndex;
            }
        }
    }
    /* close directory */
    closedir(dirToCheck);
}


/*


*/
void readFile(char *directoryName, char *fileName, struct room *aRoom) {
    char filePath[STR];
    char line[STR];
    FILE *reader;

    memset(filePath, '\0', STR);
    memset(line, '\0', STR);
    strcpy(filePath, fileName);
   /* generate filePath */
    createFileName(filePath, directoryName, aRoom);
   /* open file */
   reader = fopen(filePath, "r");
   /* check file */
   if (!reader) {
       fprintf(stderr, "Error openning file to read\n");
       exit(EXIT_FAILURE);
   }
   /* read the file */
   while(!feof(reader)) {
       fgets(line, STR, reader);
       readOneRoom(reader, aRoom, line);
       fgets(line, STR, reader);
   }
   /* close the file */
   fclose(reader);
}


/*

*/
void readOneRoom(FILE *reader,struct room *aRoom, char *line) {
    int i = 0;

    /* since name is already read, copy name to room */
    aRoom->name = getString(getData(line));
    /* now reading connection data on file */
    fgets(line, STR, reader);
    while (strstr(line, "CONNECTION") && i < CONN_SZ) {
        aRoom->connections[i] = getString(getData(line));
        ++aRoom->connectCount;
        ++i;
        fgets(line, STR, reader);
    }
    /* room type should be the line just read */
    if (strstr(line, "ROOM TYPE")) {
        aRoom->roomType = getString(getData(line));
    } else {
        fprintf(stderr, "Unable to read room type from file\n");
        exit(EXIT_FAILURE);
    }
}


/*


*/
void createFileName(char *fileName, char *directoryName, struct room *aRoom) {
    char fowardSlash[2] = "/";
    char tempPath[STR];
    
    memset(tempPath, '\0', STR);
    /* copy directory name to file name */
    strcpy(tempPath, directoryName);
    /* concatenate the forward slash */
    strcat(tempPath, fowardSlash);
    /* concatenate the file name */
    strcat(tempPath, fileName); 
    memset(fileName, '\0', STR);
    strcpy(fileName, tempPath);
}


/*

*/
char *getData(char *line) {
    char *result = NULL;

    result = strtok(line, " ");
    result = strtok(NULL, " ");
    result = strtok(NULL, "\n");
    return result;
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
    int i, j;

    struct room **list = (struct room **)malloc(SELECTED * sizeof(struct room*));
    assert(list != 0);
    for (i = 0; i < SELECTED; ++i) {
        list[i] = (struct room*)malloc(sizeof(struct room));
        assert(list[i] != 0);
        list[i]->name = NULL;
        list[i]->connectCount = 0;
        /* allocate the six connections */
        list[i]->connections = (char **)malloc(CONN_SZ * sizeof(char*));
        assert(list[i]->connections != 0); 
        for (j = 0; j < CONN_SZ; ++j) {
            list[i]->connections[j] = NULL;
        }
        list[i]->roomType = NULL;
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
                        free(list[index]->connections[j]);
                        list[index]->connections[j] = NULL;
                    }
                }
                free(list[index]->connections);
                list[index]->connections = NULL;
            }
            if (list[index]) {
                /* delete name */
                if (list[index]->name) { free(list[index]->name); }
                list[index]->name = NULL;
                /* delete room type */
                if (list[index]->roomType) { free(list[index]->roomType); }
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

