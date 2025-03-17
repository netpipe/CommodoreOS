; CC65 Assembler: Minimal Assembler for 6502
; Reads assembly from a file, translates, and writes machine code.
;Extend support for more 6502 instructions.
;Add symbol tables for labels and variables.

    .export _start
    .import _exit, _read, _write, _open, _close

    .segment "BSS"
buffer: .res 256  ; Buffer for reading lines

    .segment "DATA"
input_file:  .byte "example.asm", 0
output_file: .byte "output.bin", 0

opcode_table:
    .byte "LDA", 0, "STA", 0, "ADC", 0

    .segment "CODE"
_start:
    ; Open input file
    lda #0
    ldx #input_file
    jsr _open
    sta file_handle

    ; Open output file
    lda #1
    ldx #output_file
    jsr _open
    sta output_handle

read_line:
    ; Read a line from the input file
    ldx file_handle
    ldy #buffer
    jsr _read
    beq close_files  ; EOF?

    ; Parse and translate instruction
    jsr parse_instruction
    jsr process_instruction

    jmp read_line

close_files:
    ldx file_handle
    jsr _close

    ldx output_handle
    jsr _close

    jmp _exit  ; Exit program

parse_instruction:
    ; Parses instruction into buffer
    ldy #0
parse_loop:
    lda buffer, y
    beq parse_done
    iny
    jmp parse_loop
parse_done:
    rts

process_instruction:
    ; Match instruction in opcode table
    ldx #0
find_opcode:
    lda opcode_table, x
    cmp buffer
    beq generate_code
    inx
    cpx #3
    bne find_opcode
    jmp error_unknown

generate_code:
    lda #$A9  ; LDA immediate opcode
    jsr write_code
    rts

write_code:
    ldx output_handle
    ldy #buffer
    jsr _write
    rts

error_unknown:
    lda #$FF  ; Error indicator
    jsr _exit
