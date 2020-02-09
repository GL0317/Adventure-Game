/*
 NAME:

 SYNOPSIS:  To compile program ...
    gcc -o lindorg.adventure lindorg.adventure.c -lpthread

 AUTHOR:  Gerson Lindor Jr.
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


int main() {
    char *directoryName = NULL;
    struct room **list = NULL;

    /* set up the game */
    directoryName = openDirectory();

    /* interact with user */

    list = makeRoomList();

    if (directoryName) { free(directoryName); directoryName = NULL; }
    destroyList(list);
    return 0;
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
    char *newDirName = NULL;
    DIR *dirToCheck;
    char *target = "_room";
    int roomIndex = 0;

    /* open specified directory */
    /* check if its open */
    /* search each entry in the directory */
    /* if prefix match entry */
    /* get the attributes of the entry */
    /* read the file */
    /* get the most recent directory name */
    /* close directory */
}


/*


*/
void readFile(char *directoryName, char *fileName, struct room **list, int index) {

   /* generate filePath */
   /* open file */
   /* check file */
   /* read the file */
   /* close the file */

}


/*

*/
void readOneRoom(FILE *reader,struct room *aRoom, char *line) {
    int i = 0;

    /* since name is already read, copy name to room */
    aRoom->name = getString(getData(line));
    printf("ROOM NAME: %s", aRoom->name); /******************************************/
    /* now reading connection data on file */
    fgets(line, STR, reader);
    while (strstr(line, "CONNECTION") && i < CONN_SZ) {
        aRoom->connections[i] = getString(getData(line));
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








