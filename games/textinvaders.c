#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdint.h>
#include <c64.h>

// Define constants for screen dimensions
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

// Define player and enemy positions
int playerX, playerY;
int enemyX, enemyY;
int i;
// Function to clear the screen
void clearScreen() {
    for (i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(0, i);
        cprintf("                                        ");
    }
}

// Function to draw the player
void drawPlayer() {
    gotoxy(playerX, playerY);
    cputc(147);
}

// Function to erase the player
void erasePlayer() {
    gotoxy(playerX, playerY);
    cputc(' ');
}

// Function to draw the enemy
void drawEnemy() {
    gotoxy(enemyX, enemyY);
    cputc(152);
}

// Function to erase the enemy
void eraseEnemy() {
    gotoxy(enemyX, enemyY);
    cputc(' ');
}

// Function to update the player's position
void updatePlayer() {
    if (kbhit()) {
        char key = cgetc();
        erasePlayer();
        if (key == 'a' && playerX > 0) {
            playerX--;
        } else if (key == 'd' && playerX < SCREEN_WIDTH - 1) {
            playerX++;
        }
        drawPlayer();
    }
}

// Function to update the enemy's position
void updateEnemy() {
    eraseEnemy();
    enemyY++;
    if (enemyY >= SCREEN_HEIGHT) {
        // Respawn the enemy at the top of the screen
        enemyX = rand() % SCREEN_WIDTH;
        enemyY = 0;
    }
    drawEnemy();
}

int main() {
    // Initialize player and enemy positions
    playerX = SCREEN_WIDTH / 2;
    playerY = SCREEN_HEIGHT - 1;
    enemyX = rand() % SCREEN_WIDTH;
    enemyY = 0;

    // Initialize screen and clear it
    clrscr();

    // Main game loop
    while (1) {
        updatePlayer();
        updateEnemy();
        // Add collision detection logic here
	for (i = 0; i < 5000; i++); //small delay 500
        // Sleep for a short time to control the game speed
      //  usleep(100000);
    }

    return 0;
}

