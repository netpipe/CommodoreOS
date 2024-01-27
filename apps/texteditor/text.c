#include <stdio.h>
#include <conio.h>
#include <string.h>

#define MAX_TEXT_LENGTH 1024

char textBuffer[MAX_TEXT_LENGTH];
unsigned int cursorPos = 0;

void init_screen() {
    clrscr();
}

void update_screen() {
    clrscr();
    cputs(textBuffer);
    gotoxy(cursorPos % 40, cursorPos / 40); // Assuming 40 columns per line
}

void handle_keyboard_input() {
    if (kbhit()) {
        char ch = cgetc();
        if (ch == 13) { // Enter key
            // Logic for newline
            if (cursorPos < MAX_TEXT_LENGTH - 1) {
                textBuffer[cursorPos] = '\n';
                cursorPos++;
            }
        } else if (ch == 8) { // Backspace key
            // Logic for backspace
            if (cursorPos > 0) {
                cursorPos--;
                memmove(&textBuffer[cursorPos], &textBuffer[cursorPos + 1], strlen(textBuffer) - cursorPos);
            }
        } else {
            // Logic for regular character input
            if (cursorPos < MAX_TEXT_LENGTH - 1) {
                textBuffer[cursorPos] = ch;
                cursorPos++;
            }
        }
    }
}

void save_text_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        fputs(textBuffer, file);
        fclose(file);
    }
}

void load_text_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fgets(textBuffer, MAX_TEXT_LENGTH, file);
        fclose(file);
        cursorPos = strlen(textBuffer);
    }
}

int main() {
    init_screen();
//	textBuffer = 'gadfsda';
  load_text_from_file("test.txt");
    while (1) {
      init_screen();
        update_screen();
        handle_keyboard_input();

        // Add logic for triggering save/load
    }

    return 0;
}

