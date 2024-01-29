* = $0801
    .byte $0c, $08 ; SYS $080C

* = $0803
main:
    ; Set up screen memory
    LDA #<SCREEN_ADDRESS
    STA $2
    LDA #>SCREEN_ADDRESS
    STA $3
    LDA #40
    STA $D016

    ; Disable the default KERNAL ROM
    LDA #$35
    STA $01

    ; Call the main C function
    JMP (main)

* = $1000
    ; Additional data or code can be placed here

.include "ramdisk3.c"

