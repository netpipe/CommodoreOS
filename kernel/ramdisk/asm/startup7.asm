; Ramdisk Initialization Code
; ---------------------------

.segment "INIT"
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
    JSR MAIN

    ; Halt the system
    BRK

