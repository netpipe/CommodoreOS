#include <stdio.h>
#include <conio.h>
#include <c64.h>
#include <stdlib.h>
#include <unistd.h>


void init_screen() {
    clrscr();  // Clear the screen
}

void move_cursor(int x, int y) {
    gotoxy(x, y);
}

void print_text(const char *text, unsigned char color) {
    textcolor(color);
    cputs(text);
}

char read_char() {
    if (kbhit()) {
        return cgetc();
    }
    return 0;  // No key was pressed
}

int main() {

//int i;
    while(1) {
          init_screen();
        // Example of setting the cursor position and printing text with color
        move_cursor(5, rand() % 10 );
        print_text("Hello, Commodore 64!", COLOR_GREEN);

        // Read input
        //qchar ch = read_char();
       // if (read_char() == 'q') break; // Exit loop if 'q' is pressed
	//for (i = 0; i < 500; i++); //small delay 500
        // Add additional rendering logic here
      sleep(0.2);
    }

    return 0;
}

