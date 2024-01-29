.segment "CODE"
    .import clrscr
    .import main

    LDX #$00
loop:
    JSR clrscr   ; Clear the screen
    JSR main     ; Call the main C function
    INX
    CPX #$03     ; Replace 3 with the number of times you want the program to run
    BCC loop     ; Branch if X < 3
    BRK          ; Halt the system

