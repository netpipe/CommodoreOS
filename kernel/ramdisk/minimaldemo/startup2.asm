; Include the C64 system constants
.include "c64.inc"

.org $0801       ; Set the assembly address to $0801
    .byte $0c, $08 ; SYS $080C

.org $0803       ; Set the assembly address to $0803
main:
    ; Your minimal initialization code here

    ; Example: Clear the screen
    LDA #$00
    STA $0400
    LDA #$12
    STA $D020

    ; Call the main C function
    JMP main

; Define the screen memory location
SCREEN_ADDRESS = $0400

; End of file

