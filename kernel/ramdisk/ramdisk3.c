#include <conio.h>
#include <stdio.h>
#include <string.h>

#define RAMDISK_SIZE 4096
#define FILENAME_MAX_LENGTH 16
#define SCREEN_ADDRESS 0x0400
#define MAX_FILES 8
#define FILE_MEMORY_SIZE (RAMDISK_SIZE / MAX_FILES)

unsigned char ramdisk[RAMDISK_SIZE];

typedef struct {
    char filename[FILENAME_MAX_LENGTH];
    unsigned int offset;
    unsigned int size;
} FileEntry;

FileEntry files[MAX_FILES]; // Maximum of 8 files supported

int numFiles = 0;

void initRamdisk() {
    memset(ramdisk, 0, RAMDISK_SIZE);
    numFiles = 0;
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
    if (numFiles >= MAX_FILES) {
        // Maximum file limit reached
        return -1;
    }

    if (strlen(filename) >= FILENAME_MAX_LENGTH) {
        // Filename is too long
        return -2;
    }

    if (size > FILE_MEMORY_SIZE) {
        // File size exceeds available memory per file
        return -3;
    }

    // Find an available slot in the file table
    int fileIndex = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].size == 0) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        // No available slot in the file table
        return -4;
    }

    // Calculate the offset for this file in Ramdisk
    unsigned int offset = fileIndex * FILE_MEMORY_SIZE;

    // Write the file data to the Ramdisk
    writeToRamdisk(offset, size, data);

    // Set the file entry information
    strncpy(files[fileIndex].filename, filename, FILENAME_MAX_LENGTH);
    files[fileIndex].offset = offset;
    files[fileIndex].size = size;

    numFiles++;

    return 0;
}

int readFile(const char* filename, void* buffer, unsigned int* size) {
    for (int i = 0; i < MAX_FILES; i++) {
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
    clrscr();
    initRamdisk();

    // Create a sample file
    char dataToWrite[] = "Hello, Ramdisk!";
    if (createFile("sample.txt", dataToWrite, strlen(dataToWrite)) == 0) {
        cprintf("File 'sample.txt' created successfully!\n");
    } else {
        cprintf("Failed to create 'sample.txt'!\n");
    }

    // Read and display the contents of the sample file
    char buffer[64];
    unsigned int fileSize = 0;
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

