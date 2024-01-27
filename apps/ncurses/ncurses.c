#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

// Simple window structure
typedef struct {
    int x, y, width, height;
} Window;

// Initialize a window
void initWindow(Window* win, int x, int y, int width, int height) {
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
}

// Clear the contents of a window
void clearWindow(const Window* win) {
    int startX = win->x;
    int endX = win->x + win->width;
    int startY = win->y;
    int endY = win->y + win->height;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            gotoxy(x, y);
            cputc(' ');
        }
    }
}

// Draw a character in a window at the specified coordinates
void drawCharacter(const Window* win, int x, int y, char ch) {
    if (x >= 0 && x < win->width && y >= 0 && y < win->height) {
        gotoxy(win->x + x, win->y + y);
        cputc(ch);
    }
}

// Handle keyboard input and update the character's position
void handleInput(Window* win, int* x, int* y) {
    char key = cgetc();

    switch (key) {
        case CH_CURS_LEFT:
            if (*x > 0) {
                *x -= 1;
            }
            break;
        case CH_CURS_RIGHT:
            if (*x < win->width - 1) {
                *x += 1;
            }
            break;
        case CH_CURS_UP:
            if (*y > 0) {
                *y -= 1;
            }
            break;
        case CH_CURS_DOWN:
            if (*y < win->height - 1) {
                *y += 1;
            }
            break;
    }
}

int main() {
    clrscr();

    Window gameWindow;
    initWindow(&gameWindow, 5, 5, 30, 15);

    int playerX = 0;
    int playerY = 0;

    while (1) {
        clearWindow(&gameWindow);
        drawCharacter(&gameWindow, playerX, playerY, '@');
        handleInput(&gameWindow, &playerX, &playerY);
    }

    return 0;
}

