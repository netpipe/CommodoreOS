#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_FILENAME 15
#define MEM_SIZE 1024 * 64 // 64KB of RAM

typedef struct {
    char fileName[MAX_FILENAME];
    void* memoryLocation;
    size_t fileSize;
} File;

File fileSystem[MAX_FILES];
char memory[MEM_SIZE];
size_t nextFreeMemoryIndex = 0;

void initializeFileSystem() {
    int i;
    for (i = 0; i < MAX_FILES; ++i) {
        fileSystem[i].fileName[0] = '\0';
    }
}

void* allocateMemory(size_t size) {
    void* block;
    if (nextFreeMemoryIndex + size > MEM_SIZE) {
        return NULL;
    }
    block = &memory[nextFreeMemoryIndex];
    nextFreeMemoryIndex += size;
    return block;
}

void freeMemory(void* block, size_t size) {
    /* Advanced memory management would be required for actual freeing */
}

int createFile(const char* filename, size_t size) {
    int i;
    void* block;
    for (i = 0; i < MAX_FILES; i++) {
        if (fileSystem[i].fileName[0] == '\0') {
            strncpy(fileSystem[i].fileName, filename, MAX_FILENAME);
            block = allocateMemory(size);
            if (block == NULL) {
                printf("Error: Not enough memory.\n");
                return -1;
            }
            fileSystem[i].memoryLocation = block;
            fileSystem[i].fileSize = size;
            return i;
        }
    }
    printf("Error: File system is full.\n");
    return -1;
}

int findFileIndex(const char* filename) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (strcmp(fileSystem[i].fileName, filename) == 0) {
            return i;
        }
    }
    return -1;
}

void deleteFile(const char* filename) {
    int fileIndex = findFileIndex(filename);
    if (fileIndex != -1) {
        freeMemory(fileSystem[fileIndex].memoryLocation, fileSystem[fileIndex].fileSize);
        fileSystem[fileIndex].fileName[0] = '\0';
    } else {
        printf("Error: File not found.\n");
    }
}

void listFiles() {
    int i;
    printf("File List:\n");
    for (i = 0; i < MAX_FILES; i++) {
        if (fileSystem[i].fileName[0] != '\0') {
            printf("%s\n", fileSystem[i].fileName);
        }
    }
}

void executeCommand(const char* command) {
    char cmd[80], param1[80];
    size_t size;
    sscanf(command, "%s %s", cmd, param1);
    if (strcmp(cmd, "CREATE") == 0) {
        printf("Enter file size: ");
        scanf("%zu", &size);
        createFile(param1, size);
    } else if (strcmp(cmd, "DELETE") == 0) {
        deleteFile(param1);
    } else if (strcmp(cmd, "LIST") == 0) {
        listFiles();
    }
    /* More commands can be added here */
}

void shell() {
    char command[80];
    while (1) {
        printf("> ");
        gets(command);
        executeCommand(command);
    }
}

int main() {
    initializeFileSystem();
    printf("RAMDOS Initialized\n");
    shell();
    return 0;
}

