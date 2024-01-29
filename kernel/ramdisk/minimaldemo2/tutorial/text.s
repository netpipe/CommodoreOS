.include "c64.inc"

.segment "CODE"

    .org $0801  ; Set the program start address (for a BASIC program)

    ; Load and run the BASIC program
    lda #<basic_program
    ldy #>basic_program
    jsr $ab1e  ; KERNAL routine to execute BASIC program

    ; Infinite loop
loop:
    jmp loop

; Import the C function
.import displayHello

.segment "DATA"

basic_program:
    .asciiz "10 PRINT CHR$(147)\n"
    .asciiz "20 SYS 2061\n"
    .asciiz "30 END\n"
    .byte $00  ; Null terminator
