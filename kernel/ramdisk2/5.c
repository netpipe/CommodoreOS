#include <stdio.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_FILENAME 16
#define MEM_SIZE 1024 * 64  // Simulating 64KB of RAM
#define MAX_BLOCKS 20       // Maximum number of memory blocks

typedef struct {
    char fileName[MAX_FILENAME];
    void* memoryLocation;
    size_t fileSize;
    int inUse;
} File;

typedef struct Block {
    unsigned int startAddress;
    unsigned int size;
    struct Block* next;
} Block;

File fileSystem[MAX_FILES];
unsigned char memory[MEM_SIZE];  // Simulated memory
Block blocks[MAX_BLOCKS];
Block* freeList;

void initializeMemoryManager() {
    freeList = &blocks[0];
    freeList->startAddress = 0x0801; // Start after BASIC interpreter
    freeList->size = MEM_SIZE - freeList->startAddress;
    freeList->next = NULL;

    for (int i = 1; i < MAX_BLOCKS; i++) {
        blocks[i].next = NULL;
    }
}

void* allocateMemory(unsigned int size) {
    Block *current = freeList, *previous = NULL;
    while (current != NULL) {
        if (current->size >= size) {
            unsigned int allocAddress = current->startAddress;
            current->startAddress += size;
            current->size -= size;

            if (current->size == 0) {
                if (previous == NULL) {
                    freeList = current->next;
                } else {
                    previous->next = current->next;
                }
            }
            return &memory[allocAddress];
        }
        previous = current;
        current = current->next;
    }
    return NULL; // No sufficient free block found
}

void freeMemory(void* block, unsigned int size) {
    unsigned int address = (unsigned int)block - (unsigned int)&memory[0];
    Block* newBlock = NULL;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].next == NULL) {
            newBlock = &blocks[i];
            break;
        }
    }

    if (newBlock == NULL) return; // No free block structures available

    newBlock->startAddress = address;
    newBlock->size = size;
    newBlock->next = freeList;
    freeList = newBlock;
}

void initializeFileSystem() {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        fileSystem[i].inUse = 0;
    }
}

int createFile(const char* filename, size_t size) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (!fileSystem[i].inUse) {
            strncpy(fileSystem[i].fileName, filename, MAX_FILENAME - 1);
            fileSystem[i].fileName[MAX_FILENAME - 1] = '\0';
            fileSystem[i].memoryLocation = allocateMemory(size);
            if (fileSystem[i].memoryLocation == NULL) {
                printf("Error: Not enough memory.\n");
                return -1;
            }
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

int main() {
    char command[80], filename[MAX_FILENAME];
    size_t size;

    initializeMemoryManager();
    initializeFileSystem();
    printf("RAMDOS Initialized. Enter commands.\n");

    while (1) {
        printf("> ");
        if (scanf("%79s", command) != 1) break;

        if (strcmp(command, "CREATE") == 0) {
            printf("Enter filename: ");
            if (scanf("%15s", filename) != 1) break;
            printf("Enter size: ");
            if (scanf("%zu", &size) != 1) break;
            createFile(filename, size);
        } else if (strcmp(command, "DELETE") == 0) {
            printf("Enter filename: ");
            if (scanf("%15s", filename) != 1) break;
            deleteFile(filename);
        } else if (strcmp(command, "LIST") == 0) {
            listFiles();
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}

