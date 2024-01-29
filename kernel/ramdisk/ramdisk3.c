#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RAMDISK_SIZE 4096
#define FILENAME_MAX_LENGTH 16
#define SCREEN_ADDRESS 0x0400
#define MAX_FILES 8

unsigned char ramdisk[RAMDISK_SIZE];

typedef struct {
    char filename[FILENAME_MAX_LENGTH];
    unsigned int offset;
    unsigned int size;
} FileEntry;

FileEntry files[MAX_FILES]; // Maximum of 8 files supported

int numFiles = 0;

// Simple memory management structures and functions
#define MEMORY_BLOCK_SIZE 16

typedef struct {
    unsigned int offset;
    unsigned int size;
} MemoryBlock;

MemoryBlock memoryBlocks[MAX_FILES];

void initializeMemory() {
int i;
    for ( i = 0; i < MAX_FILES; i++) {
        memoryBlocks[i].offset = i * MEMORY_BLOCK_SIZE;
        memoryBlocks[i].size = 0;
    }
}

int allocateMemory(unsigned int size) {
int i;
    for ( i = 0; i < MAX_FILES; i++) {
        if (memoryBlocks[i].size == 0 && memoryBlocks[i].offset + size <= RAMDISK_SIZE) {
            memoryBlocks[i].size = size;
            return memoryBlocks[i].offset;
        }
    }
    return -1; // No available memory block
}

void deallocateMemory(unsigned int offset) {
int i;
    for (i = 0; i < MAX_FILES; i++) {
        if (memoryBlocks[i].offset == offset) {
            memoryBlocks[i].size = 0;
            break;
        }
    }
}

void writeToRamdisk(unsigned int offset, unsigned int length, const void* data) {
    if (offset + length > RAMDISK_SIZE) {
        // Handle out-of-bounds access error
        return;
    }

    memcpy(&ramdisk[offset], data, length);
}

void readFromRamdisk(unsigned int offset, unsigned int length, void* buffer) {
    if (offset + length > RAMDISK_SIZE) {
        // Handle out-of-bounds access error
        return;
    }

    memcpy(buffer, &ramdisk[offset], length);
}

int createFile(const char* filename, const void* data, unsigned int size) {
int offset;
    if (numFiles >= MAX_FILES) {
        // Maximum file limit reached
        return -1;
    }

    if (strlen(filename) >= FILENAME_MAX_LENGTH) {
        // Filename is too long
        return -2;
    }

    offset = allocateMemory(size);
    if (offset < 0) {
        // Not enough free memory on the Ramdisk
        return -3;
    }

    // Write the file data to the allocated memory on the Ramdisk
    writeToRamdisk(offset, size, data);

    // Set the file entry information
    strncpy(files[numFiles].filename, filename, FILENAME_MAX_LENGTH);
    files[numFiles].offset = offset;
    files[numFiles].size = size;

    numFiles++;

    return 0;
}

int deleteFile(const char* filename) {
int i;
    for (i = 0; i < numFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {
            deallocateMemory(files[i].offset);
            memset(&files[i], 0, sizeof(FileEntry));
            numFiles--;
            return 0; // File deleted
        }
    }
    return -1; // File not found
}

int readFile(const char* filename, void* buffer, unsigned int* size) {
int i;
    for (i = 0; i < numFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {
            if (buffer != NULL) {
                readFromRamdisk(files[i].offset, files[i].size, buffer);
            }
            if (size != NULL) {
                *size = files[i].size;
            }
            return 0; // File found
        }
    }
    return -1; // File not found
}

int main() {
    char buffer[64];
    unsigned int fileSize = 0;
    char dataToWrite[] = "Hello, Ramdisk!";
    clrscr();
    initializeMemory();

    // Create a sample file

    if (createFile("sample.txt", dataToWrite, strlen(dataToWrite)) == 0) {
        cprintf("File 'sample.txt' created successfully!\n");
    } else {
        cprintf("Failed to create 'sample.txt'!\n");
    }

    // Delete the sample file
    if (deleteFile("sample.txt") == 0) {
        cprintf("File 'sample.txt' deleted!\n");
    } else {
        cprintf("Failed to delete 'sample.txt'!\n");
    }

    // Read and display the contents of the deleted file (should fail)

    if (readFile("sample.txt", buffer, &fileSize) == 0) {
        cprintf("Contents of 'sample.txt':\n");
        buffer[fileSize] = '\0'; // Null-terminate for printing
        cputs(buffer);
        cprintf("\nFile size: %d bytes\n", fileSize);
    } else {
        cprintf("Failed to read 'sample.txt'!\n");
    }

    // Wait for a key press and exit
    cgetc();
    return 0;
}

