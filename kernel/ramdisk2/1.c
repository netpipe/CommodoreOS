#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_FILENAME 15
#define MEM_SIZE 1024 * 64 // 64KB of RAM, for example

typedef struct {
    char fileName[MAX_FILENAME];
    void* memoryLocation;
    size_t fileSize;
} File;

File fileSystem[MAX_FILES];
char memory[MEM_SIZE];

void initializeFileSystem() {
    for (int i = 0; i < MAX_FILES; i++) {
        fileSystem[i].fileName[0] = '\0';
    }
}

void* allocateMemory(size_t size) {
    static size_t memIndex = 0;
    if (memIndex + size > MEM_SIZE) return NULL;
    void* block = &memory[memIndex];
    memIndex += size;
    return block;
}

void freeMemory(void* block) {
    // Simplified version - actual implementation should handle memory management
}

int findFreeFileIndex() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fileSystem[i].fileName[0] == '\0') {
            return i;
        }
    }
    return -1;
}

void copyFile(const char* sourceName, const char* destName) {
    int sourceIndex = -1, destIndex = findFreeFileIndex();
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fileSystem[i].fileName, sourceName) == 0) {
            sourceIndex = i;
            break;
        }
    }

    if (sourceIndex == -1 || destIndex == -1) {
        printf("Copy error: file not found or memory full\n");
        return;
    }

    void* newLocation = allocateMemory(fileSystem[sourceIndex].fileSize);
    if (newLocation == NULL) {
        printf("Copy error: insufficient memory\n");
        return;
    }

    memcpy(newLocation, fileSystem[sourceIndex].memoryLocation, fileSystem[sourceIndex].fileSize);
    strcpy(fileSystem[destIndex].fileName, destName);
    fileSystem[destIndex].memoryLocation = newLocation;
    fileSystem[destIndex].fileSize = fileSystem[sourceIndex].fileSize;
}

void moveFile(const char* sourceName, const char* destName) {
    // Implement file move logic
}

void renameFile(const char* oldName, const char* newName) {
    // Implement file rename logic
}

void executeCommand(const char* command) {
    char cmd[80], param1[80], param2[80];
    sscanf(command, "%s %s %s", cmd, param1, param2);
    if (strcmp(cmd, "COPY") == 0) {
        copyFile(param1, param2);
    } else if (strcmp(cmd, "MOVE") == 0) {
        moveFile(param1, param2);
    } else if (strcmp(cmd, "RENAME") == 0) {
        renameFile(param1, param2);
    }
    // Handle other commands
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
    printf("RAMDOS Enhanced Initialized\n");
    shell();
    return 0;
}

