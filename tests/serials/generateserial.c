#include <stdio.h>
#include <string.h>

// Define the character set for alphanumeric serial numbers
const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// Function to generate serial numbers recursively
void generateSerialNumbers(char* currentSerial, int currentIndex, int serialLength) {
    if (currentIndex == serialLength) {
        printf("%s\n", currentSerial);
        return;
    }

    for (int i = 0; i < strlen(charset); i++) {
        currentSerial[currentIndex] = charset[i];
        generateSerialNumbers(currentSerial, currentIndex + 1, serialLength);
    }
}

int main() {
    const int serialLength = 4; // Set the desired length of the serial number
    char currentSerial[serialLength + 1]; // +1 for the null terminator

    memset(currentSerial, 0, sizeof(currentSerial)); // Initialize the serial number buffer

    generateSerialNumbers(currentSerial, 0, serialLength);

    return 0;
}

