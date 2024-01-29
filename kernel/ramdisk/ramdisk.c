#include <conio.h>
#include <stdio.h>
#include <string.h>

#define RAMDISK_SIZE 4096
#define SCREEN_ADDRESS 0x0400

unsigned char ramdisk[RAMDISK_SIZE];

void initRamdisk() {
    memset(ramdisk, 0, RAMDISK_SIZE);
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

int main() {
    clrscr();
    initRamdisk();

    // Write some data to the Ramdisk
    char dataToWrite[] = "Hello, Ramdisk!";
    writeToRamdisk(0, strlen(dataToWrite), dataToWrite);

    // Read data from the Ramdisk
    char buffer[16];
    readFromRamdisk(0, sizeof(buffer), buffer);

    // Display the read data on the screen
    gotoxy(5, 10);
    cprintf("Data from Ramdisk: %s", buffer);

    // Wait for a key press and exit
    cgetc();
    return 0;
}

