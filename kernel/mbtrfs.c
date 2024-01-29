#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 16
#define FILE_CONTENT_SIZE 1024  // Maximum uncompressed file size
#define STORAGE_SIZE 65536      // Total storage size (for demo purposes)

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint16_t start;             // Start position in the storage
    uint16_t length;            // Length of compressed data
    uint16_t original_length;   // Length of original data
} FileEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    uint8_t storage[STORAGE_SIZE]; // Simplified storage
} FileSystem;

// RLE Compression Function
void rleCompress(const uint8_t* input, uint16_t inputSize, uint8_t* output, uint16_t* outputSize) {
    uint16_t inIdx = 0, outIdx = 0;
    while (inIdx < inputSize) {
        uint8_t count = 1;
        while (inIdx + count < inputSize && count < 255 && input[inIdx] == input[inIdx + count]) {
            count++;
        }
        output[outIdx++] = count;
        output[outIdx++] = input[inIdx];
        inIdx += count;
    }
    *outputSize = outIdx;
}

// RLE Decompression Function
void rleDecompress(const uint8_t* input, uint16_t inputSize, uint8_t* output, uint16_t* outputSize) {
    uint16_t inIdx = 0, outIdx = 0;
    while (inIdx < inputSize) {
        uint8_t count = input[inIdx++];
        uint8_t value = input[inIdx++];
        for (uint8_t i = 0; i < count; i++) {
            output[outIdx++] = value;
        }
    }
    *outputSize = outIdx;
}

// Function to Initialize the FileSystem
void initializeFileSystem(FileSystem* fs) {
    memset(fs, 0, sizeof(FileSystem));
}

// Function to Create a File in the FileSystem
int createFile(FileSystem* fs, const char* filename, const uint8_t* data, uint16_t dataSize) {
    // Find an empty file entry
    int fileIndex = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs->files[i].filename[0] == '\0') {
            fileIndex = i;
            break;
        }
    }
    if (fileIndex == -1) {
        return -1; // No available file entry
    }

    // Compress the data
    uint8_t compressedData[FILE_CONTENT_SIZE];
    uint16_t compressedSize;
    rleCompress(data, dataSize, compressedData, &compressedSize);

    // Check for available space in storage
    // For simplicity, this example does not handle fragmentation
    if (compressedSize > STORAGE_SIZE) {
        return -2; // Not enough space
    }

    // Write data to storage
    memcpy(fs->storage, compressedData, compressedSize);

    // Update file entry
    strncpy(fs->files[fileIndex].filename, filename, MAX_FILENAME_LENGTH - 1);
    fs->files[fileIndex].start = 0; // Starting at 0 for simplicity
    fs->files[fileIndex].length = compressedSize;
    fs->files[fileIndex].original_length = dataSize;

    return 0;
}

// Function to Read a File from the FileSystem
int readFile(FileSystem* fs, const char* filename, uint8_t* buffer, uint16_t bufferSize) {
    // Find the file entry
    int fileIndex = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs->files[i].filename, filename) == 0) {
            fileIndex = i;
            break;
        }
    }
    if (fileIndex == -1) {
        return -1; // File not found
    }

    // Decompress the data
    uint16_t outputSize;
    rleDecompress(fs->storage + fs->files[fileIndex].start, fs->files[fileIndex].length, buffer, &outputSize);

    if (outputSize > bufferSize) {
        return -2; // Buffer too small
    }

    return 0;
}



int main() {
    FileSystem fs;
    initializeFileSystem(&fs);

    // Example file data
    uint8_t fileData[FILE_CONTENT_SIZE] = { /* file content */ };
    uint16_t fileSize = /* size of fileData */;

    if (createFile(&fs, "example.txt", fileData, fileSize) != 0) {
        printf("Error creating file.\n");
    }

    uint8_t buffer[FILE_CONTENT_SIZE];
    if (readFile(&fs, "example.txt", buffer, sizeof(buffer)) != 0) {
        printf("Error reading file.\n");
    }

    // Use buffer...
    return 0;
}



