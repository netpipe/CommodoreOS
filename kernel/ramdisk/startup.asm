; Include the C64 system constants
.include "c64.inc"

.org $0801
    .byte $0c, $08 ; SYS $080C

.org $0803
main:
    ; Clear the screen
    LDA #$00
    STA $0400
    LDA #$12
    STA $D020

    ; Set screen memory address
    LDA #<SCREEN_ADDRESS
    STA $07
    LDA #>SCREEN_ADDRESS
    STA $08

    ; Print "Hello, World!" to the screen
    LDX #0
loop:
    LDA MESSAGE, X
    BEQ done
    STA $0400, X
    INX
    JMP loop
done:
    ; Wait for key press
    JSR $FFE4

    ; Return to BASIC
    RTS

SCREEN_ADDRESS = $0400
MESSAGE:
    .text "Hello, World!"

