#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>

#define MAX_FILES 10
#define MAX_FILENAME 16
#define MEM_SIZE 1024 * 64 // C64 RAM Size

typedef struct {
    char fileName[MAX_FILENAME];
    void* memoryLocation;
    size_t fileSize;
    int inUse;
} File;

File fileSystem[MAX_FILES];
unsigned char memory[MEM_SIZE];
unsigned int nextFreeMemoryIndex = 0x0801; // Start after BASIC interpreter

void initializeFileSystem() {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        fileSystem[i].fileName[0] = '\0';
        fileSystem[i].inUse = 0;
    }
}

void* allocateMemory(size_t size) {
    void* block;
    if (nextFreeMemoryIndex + size > MEM_SIZE) {
        return NULL;
    }
    block = (void*)&memory[nextFreeMemoryIndex];
    nextFreeMemoryIndex += size;
    return block;
}

void freeMemory(void* block, size_t size) {
    // Simplified version - actual implementation should manage memory blocks
}

int createFile(const char* filename, size_t size) {
    int i;
    void* block;
    for (i = 0; i < MAX_FILES; i++) {
        if (!fileSystem[i].inUse) {
            strncpy(fileSystem[i].fileName, filename, MAX_FILENAME);
            block = allocateMemory(size);
            if (block == NULL) {
                printf("Error: Not enough memory.\n");
                return -1;
            }
            fileSystem[i].memoryLocation = block;
            fileSystem[i].fileSize = size;
            fileSystem[i].inUse = 1;
            return i;
        }
    }
    printf("Error: File system is full.\n");
    return -1;
}

void deleteFile(const char* filename) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (fileSystem[i].inUse && strcmp(fileSystem[i].fileName, filename) == 0) {
            freeMemory(fileSystem[i].memoryLocation, fileSystem[i].fileSize);
            fileSystem[i].inUse = 0;
            fileSystem[i].fileName[0] = '\0';
            return;
        }
    }
    printf("Error: File not found.\n");
}

void listFiles() {
    int i;
    printf("File List:\n");
    for (i = 0; i < MAX_FILES; i++) {
        if (fileSystem[i].inUse) {
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
    // Add more commands here
}

void shell() {
    char command[80];
    cbm_k_clrscr(); // Clear the screen using KERNAL routine
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        executeCommand(command);
    }
}

int main() {
    initializeFileSystem();
    printf("RAMDOS for C64 Initialized\n");
    shell();
    return 0;
}

