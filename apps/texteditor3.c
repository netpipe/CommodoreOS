#include <conio.h>
#include <tgi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mouse.h>

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25
#define MAX_TEXT_ROWS 100
#define MAX_TEXT_COLS 80
#define MAX_TEXT_SIZE (MAX_TEXT_ROWS * MAX_TEXT_COLS)


char textBuffer[MAX_TEXT_SIZE];
int cursorX, cursorY;
int selectionStartX, selectionStartY, selectionEndX, selectionEndY;
int textRows, textCols;
char filename[] = "textfile.txt";

 void mouse_init();
 void update_mouse();
 int mouse_x();
 int mouse_y();
 unsigned char mouse_buttons();
 unsigned char buttons;
    int mx, my;
    
    struct mouse_box full_box, small_box;
    unsigned char width, height;
            struct mouse_info info;
        
   
        
int mouse_y()  {
  //      cprintf (" X  = %3d\r\n", info.pos.x);    
return info.pos.x;
}

int mouse_x()  {
 //   cprintf (" Y  = %3d\r\n", info.pos.y);
return info.pos.y;
}

void update_mouse(){ 
    mouse_info(&info);
   // gotoxy(40, 2);
}  
unsigned char mouse_buttons(){
     //   cprintf (" B1 = %c\r\n", (info.buttons & MOUSE_BTN_LEFT) ? 'v'  : '^');
     //   cprintf (" B2 = %c", (info.buttons & MOUSE_BTN_RIGHT) ? 'v'  : '^');
}
    
void mouse_where(int* x, int* y, unsigned char* buttons) {
    // Retrieve mouse coordinates and button states using the hypothetical library functions
    update_mouse();
    *x = mouse_x();
    *y = mouse_y();
    *buttons = mouse_buttons();
}

// Function to check and report errors
void CheckError(const char* message, unsigned char errorCode) {
    if (errorCode != MOUSE_ERR_OK) {
        cprintf("%s: Error %u\n", message, errorCode);
        exit(EXIT_FAILURE);
    }
}

void mouse_init(){
clrscr();
     cursor (0);
    //const char* mouse_name = mouse_stddrv;
    CheckError ("mouse_load_driver",             mouse_load_driver(&mouse_def_callbacks,mouse_stddrv));
     
    mouse_show();
    mouse_getbox (&full_box);
    screensize (&width, &height);
}
     
int i;
void mouse_select_start();
void mouse_select_end();

void clearScreen() {
    tgi_clear();
}
int row,col;
void drawText() {
    int startRow = cursorY - SCREEN_HEIGHT / 2;
    int endRow = startRow + SCREEN_HEIGHT;

    if (startRow < 0) {
        startRow = 0;
        endRow = SCREEN_HEIGHT;
    }

    if (endRow > textRows) {
        endRow = textRows;
        startRow = endRow - SCREEN_HEIGHT;
    }

    for ( row = startRow; row < endRow; row++) {
        for ( col = 0; col < SCREEN_WIDTH; col++) {
            char character = textBuffer[row * MAX_TEXT_COLS + col];

            if (row == cursorY && col == cursorX) {
                tgi_setcolor(COLOR_WHITE);
     //           tgi_setbgcolor(COLOR_BLACK);
            } else if (row >= selectionStartY && row <= selectionEndY &&
                       col >= selectionStartX && col <= selectionEndX) {
                tgi_setcolor(COLOR_BLACK);
      //          tgi_setbgcolor(COLOR_WHITE);
            } else {
                tgi_setcolor(COLOR_WHITE);
      //          tgi_setbgcolor(COLOR_BLACK);
            }

            tgi_outtextxy(col * 8, (row - startRow) *, &character);
        }
    }
}

void drawCursor() {
    tgi_setcolor(COLOR_WHITE);
 //   tgi_setbgcolor(COLOR_BLACK);
    tgi_outtextxy(cursorX * 8, (cursorY % SCREEN_HEIGHT) * 8, "_");
}

void handleInput() {
    if (kbhit()) {
        char key = cgetc();

        switch (key) {
            case CH_CURS_UP:
                if (cursorY > 0) cursorY--;
                break;
            case CH_CURS_DOWN:
                if (cursorY < textRows - 1) cursorY++;
                break;
            case CH_CURS_LEFT:
                if (cursorX > 0) cursorX--;
                break;
            case CH_CURS_RIGHT:
                if (cursorX < textCols - 1) cursorX++;
                break;
            case CH_ENTER:
                // Handle line breaks
                if (textRows < MAX_TEXT_ROWS) {
                    memmove(textBuffer + (cursorY + 1) * MAX_TEXT_COLS, textBuffer + cursorY * MAX_TEXT_COLS + cursorX,
                            (textRows - cursorY) * MAX_TEXT_COLS - cursorX);
                    cursorY++;
                    cursorX = 0;
                    textRows++;
                }
                break;
            case CH_DEL:
                // Handle deletion
                if (cursorX < textCols - 1) {
                    memmove(textBuffer + cursorY * MAX_TEXT_COLS + cursorX, textBuffer + cursorY * MAX_TEXT_COLS + cursorX + 1,
                            textCols - cursorX - 1);
                } else if (cursorY < textRows - 1) {
                    memmove(textBuffer + cursorY * MAX_TEXT_COLS + cursorX, textBuffer + (cursorY + 1) * MAX_TEXT_COLS, (textRows - cursorY - 1) * MAX_TEXT_COLS);
                    textRows--;
                }
                break;
        case CH_F6: // Copy
            selectionStartX = cursorX;
            selectionStartY = cursorY;
            selectionEndX = cursorX;
            selectionEndY = cursorY;
            break;
       // case CH_F4: // Cut
       //     selectionStartX = cursorX;
         //   selectionStartY = cursorY;
        //    selectionEndX = cursorX;
        //    selectionEndY = cursorY;
       //     break;
        case CH_F8: // Paste
            if (selectionStartY >= 0 && selectionEndY < textRows && selectionStartX >= 0 && selectionEndX < textCols) {
                int selectedRows = selectionEndY - selectionStartY + 1;
                int selectedCols = selectionEndX - selectionStartX + 1;

                if (cursorX + selectedCols <= textCols && cursorY + selectedRows <= textRows) {
                    for ( row = 0; row < selectedRows; row++) {
                        memcpy(textBuffer + MAX_TEXT_COLS * (cursorY + row) + cursorX,
                               textBuffer + MAX_TEXT_COLS * (selectionStartY + row) + selectionStartX,
                               selectedCols);
                    }
                    cursorX += selectedCols;
                    cursorY += selectedRows;
                }
            }
            break;
  //      case CH_F5: // Select
 //           selectionStartX = cursorX;
 //           selectionStartY = cursorY;
 //           break;
 //       case CH_F4: // End selection
 //           selectionEndX = cursorX;
 //           selectionEndY = cursorY;
 //           break;
            case 133: // Mouse left button press
                mouse_select_start();
                break;
            case 134: // Mouse left button release
                mouse_select_end();
                break;
            case 135: // Mouse right button press
                break;
            case 136: // Mouse right button release
                break;
            default:
                // Handle typing
                if (cursorX < textCols - 1) {
                    memmove(textBuffer + cursorY * MAX_TEXT_COLS + cursorX + 1, textBuffer + cursorY * MAX_TEXT_COLS + cursorX,
                            textCols - cursorX - 1);
                }
                textBuffer[cursorY * MAX_TEXT_COLS + cursorX] = key;
                cursorX++;
        }
    }


    mouse_where(&mx, &my, &buttons);

    if (buttons & 1) {
        // Left mouse button pressed
        mouse_select_start();
    } else if (buttons & 2) {
        // Right mouse button pressed
    } else {
        // No mouse button pressed
        mouse_select_end();
    }
}

void saveToFile() {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fwrite(textBuffer, sizeof(char), textRows * MAX_TEXT_COLS, file);

    fclose(file);
}

void loadFromFile() {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    textRows = fread(textBuffer, sizeof(char), MAX_TEXT_SIZE, file) / MAX_TEXT_COLS;

    fclose(file);
}

void mouse_select_start() {
    mouse_where(&selectionStartX, &selectionStartY, NULL);
}

void mouse_select_end() {
    mouse_where(&selectionEndX, &selectionEndY, NULL);
}

int main() {
    tgi_install(&tgi_static_stddrv);

   tgi_init();
   mouse_init();
        
    cursorX = 0;
    cursorY = 0;
    selectionStartX = -1;
    selectionStartY = -1;
    selectionEndX = -1;
    selectionEndY = -1;
    textRows = 1;
    textCols = MAX_TEXT_COLS;

    clearScreen();

    loadFromFile();

    while (1) {
    tgi_clear();
       //     struct mouse_info info;
     //   mouse_info(&info);
        drawText();
        drawCursor();
        handleInput();
        


        // Clear the previous output
    //    gotoxy(0, 0);
     //   cclear(40);

        // Display the mouse coordinates and button state
        //gotoxy(0, 2);

        
        if (cursorY >= textRows) {
            cursorY = textRows - 1;
        }

        if (cursorX >= textCols) {
            cursorX = textCols - 1;
        }

        if (cursorY >= MAX_TEXT_ROWS) {
            memmove(textBuffer, textBuffer + MAX_TEXT_COLS, MAX_TEXT_COLS * (MAX_TEXT_ROWS - 1));
            cursorY--;
            textRows--;
        }

        if (cursorX >= MAX_TEXT_COLS) {
            cursorX = MAX_TEXT_COLS - 1;
        }

        if (kbhit()) {
            if (cgetc() == 19) { // CTRL + S
                saveToFile();
            }
        }
                for (i = 0; i < 50; i++); //small delay 500
                tgi_done();
    }

    tgi_done();
    
        mouse_hide();
  //  mouse_unload_drv();
    CheckError ("mouse_unload", mouse_unload ());

    return 0;
}

