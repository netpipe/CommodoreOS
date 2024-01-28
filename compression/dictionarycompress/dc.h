#ifndef DICTIONARY_COMPRESSION_H
#define DICTIONARY_COMPRESSION_H

#include <stdint.h>
#include <string.h>

#define MAX_DICT_SIZE 256

// Structure for a dictionary entry
typedef struct {
    char sequence[MAX_DICT_SIZE];
    uint8_t code;
} DictionaryEntry;

// Function to initialize the dictionary
void initDictionary(DictionaryEntry dictionary[], int dictSize) {
    for (int i = 0; i < dictSize; i++) {
        memset(dictionary[i].sequence, 0, MAX_DICT_SIZE);
        dictionary[i].code = i;
    }
}

// Function to compress text using a dictionary
// Input: src - Pointer to the source text (null-terminated string)
//        dest - Pointer to the destination buffer (must be preallocated)
// Return: Size of the compressed data in the destination buffer
int dictionaryCompress(const char* src, uint8_t* dest) {
    int dictSize = MAX_DICT_SIZE;
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    initDictionary(dictionary, dictSize);

    int srcIndex = 0;
    int destIndex = 0;
    char currentSequence[MAX_DICT_SIZE] = {0};
    int sequenceLength = 0;

    while (src[srcIndex] != '\0') {
        currentSequence[sequenceLength++] = src[srcIndex++];

        int foundIndex = -1;
        for (int i = 0; i < dictSize; i++) {
            if (strcmp(dictionary[i].sequence, currentSequence) == 0) {
                foundIndex = i;
                break;
            }
        }

        if (foundIndex == -1) {
            // Add the new sequence to the dictionary
            dictionary[dictSize].code = dictSize;
            strncpy(dictionary[dictSize].sequence, currentSequence, MAX_DICT_SIZE);
            dest[destIndex++] = dictionary[dictSize - 1].code;

            // Reset the current sequence
            memset(currentSequence, 0, MAX_DICT_SIZE);
            sequenceLength = 0;

            dictSize++;

            if (dictSize >= MAX_DICT_SIZE) {
                // Dictionary full, reset it
                initDictionary(dictionary, dictSize);
                dictSize = MAX_DICT_SIZE;
            }
        }
    }

    // Handle the last sequence
    dest[destIndex++] = dictionary[dictSize - 1].code;

    return destIndex;
}

// Function to decompress text compressed with a dictionary
// Input: src - Pointer to the compressed data
//        dest - Pointer to the destination buffer (must be preallocated)
void dictionaryDecompress(const uint8_t* src, char* dest) {
    int dictSize = MAX_DICT_SIZE;
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    initDictionary(dictionary, dictSize);

    int srcIndex = 0;
    int destIndex = 0;
    uint8_t currentCode = src[srcIndex++];
    char currentSequence[MAX_DICT_SIZE] = {0};
    int sequenceLength = 0;

    while (currentCode != '\0') {
        if (currentCode < dictSize) {
            // Copy sequence from dictionary
            strncpy(currentSequence, dictionary[currentCode].sequence, MAX_DICT_SIZE);
            sequenceLength = strlen(currentSequence);
        } else {
            // Copy the character itself
            currentSequence[sequenceLength++] = currentCode;
        }

        strcpy(&dest[destIndex], currentSequence);
        destIndex += sequenceLength;

        if (src[srcIndex] != '\0') {
            // Add the new sequence to the dictionary
            dictionary[dictSize].code = dictSize;
            strncpy(dictionary[dictSize].sequence, currentSequence, MAX_DICT_SIZE);

            dictSize++;
        }

        currentCode = src[srcIndex++];
        memset(currentSequence, 0, MAX_DICT_SIZE);
        sequenceLength = 0;
    }

    dest[destIndex] = '\0'; // Null-terminate the decompressed string
}

#endif // DICTIONARY_COMPRESSION_H

