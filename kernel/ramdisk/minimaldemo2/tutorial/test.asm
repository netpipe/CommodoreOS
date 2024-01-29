.include "c64.inc"

.segment "CODE"

    .org $1000  ; Set the program start address

start:
    ; Initialize the system
    sei
    cld
    ldx #$FF
    txs
    inx
    stx $01

    ; Set screen memory to $0400
    lda #$04
    sta $d018

    ; Load the address of the message
    ldx #<message
    ldy #>message

    ; Call KERNAL routine to print the message
    jsr $ab1e  ; KERNAL routine to print null-terminated string

    ; Infinite loop
    jmp *

.segment "DATA"

message:
    .asciiz "Hello, World!"  ; Null-terminated string

    .end

