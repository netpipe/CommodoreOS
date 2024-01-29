#include <stdio.h>
#include <string.h>
#include <cbm.h>

#define MEM_SIZE 1024 * 64 // 64KB of RAM
#define MAX_BLOCKS 20 // Max number of memory blocks we can track

typedef struct Block {
    unsigned int startAddress;
    unsigned int size;
    struct Block* next;
} Block;

unsigned char memory[MEM_SIZE];
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
    Block* current = freeList;
    Block* previous = NULL;

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

    // Find a free block structure
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (blocks[i].next == NULL) {
            newBlock = &blocks[i];
            break;
        }
    }

    if (newBlock == NULL) return; // No free block structures

    newBlock->startAddress = address;
    newBlock->size = size;
    newBlock->next = freeList;
    freeList = newBlock;
}

// Rest of your RAMDOS code...

