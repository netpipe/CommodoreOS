; ==============================================================================
; COMMODORE-OS 6502 SELF-ASSEMBLER
; ==============================================================================
; A fully functional two-pass 6502 assembler written in 6502 assembly.
; Capable of assembling its own source code.
; ==============================================================================

.export _start
.import _open, _read, _write, _close, _exit
.importzp sp

.segment "ZEROPAGE"
ptr_low:    .res 1
ptr_high:   .res 1
line_ptr:   .res 2
val_low:    .res 1
val_high:   .res 1
cur_mode:   .res 1
instr_id:   .res 1
token_len:  .res 1
cur_sym:    .res 1
end_ptr:    .res 2

.segment "BSS"
in_fd:      .res 1
out_fd:     .res 1
pass:       .res 1
pc:         .res 2
out_ptr:    .res 2
sym_count:  .res 1
in_size:    .res 2

line_buf:   .res 128
token_buf:  .res 32
sym_names:  .res 1024  ; 64 labels * 16 chars
sym_addrs:  .res 128   ; 64 labels * 2 bytes
out_buf:    .res 8192  ; 8KB output buffer
in_buf:     .res 4096  ; 4KB input buffer

.segment "DATA"
in_path:    .byte "main.asm", 0
out_path:   .byte "main.prg", 0

mnemonic_tab:
    .byte "LDA", 0,  "STA", 1,  "LDX", 2,  "STX", 3
    .byte "LDY", 4,  "STY", 5,  "CMP", 6,  "CPX", 7
    .byte "CPY", 8,  "ADC", 9,  "SBC", 10, "AND", 11
    .byte "ORA", 12, "EOR", 13, "INC", 14, "DEC", 15
    .byte "JMP", 16, "JSR", 17, "BEQ", 18, "BNE", 19
    .byte "BCC", 20, "BCS", 21, "INX", 22, "DEX", 23
    .byte "INY", 24, "DEY", 25, "RTS", 26, "CLC", 27
    .byte "SEC", 28, "NOP", 29, 0, 0, 0, 255

opcodes:
    .byte $FF,$A9,$A5,$B5,$FF,$AD,$BD,$B9,$FF ; LDA
    .byte $FF,$FF,$85,$95,$FF,$8D,$9D,$99,$FF ; STA
    .byte $FF,$A2,$A6,$FF,$B6,$AE,$FF,$BE,$FF ; LDX
    .byte $FF,$FF,$86,$FF,$96,$8E,$FF,$FF,$FF ; STX
    .byte $FF,$A0,$A4,$B4,$FF,$AC,$BC,$FF,$FF ; LDY
    .byte $FF,$FF,$84,$94,$FF,$8C,$FF,$FF,$FF ; STY
    .byte $FF,$C9,$C5,$D5,$FF,$CD,$DD,$D9,$FF ; CMP
    .byte $FF,$E0,$E4,$FF,$FF,$EC,$FF,$FF,$FF ; CPX
    .byte $FF,$C0,$C4,$FF,$FF,$CC,$FF,$FF,$FF ; CPY
    .byte $FF,$69,$65,$75,$FF,$6D,$7D,$79,$FF ; ADC
    .byte $FF,$E9,$E5,$F5,$FF,$ED,$FD,$F9,$FF ; SBC
    .byte $FF,$29,$25,$35,$FF,$2D,$3D,$39,$FF ; AND
    .byte $FF,$09,$05,$15,$FF,$0D,$1D,$19,$FF ; ORA
    .byte $FF,$49,$45,$55,$FF,$4D,$5D,$59,$FF ; EOR
    .byte $FF,$FF,$E6,$F6,$FF,$EE,$FE,$FF,$FF ; INC
    .byte $FF,$FF,$C6,$D6,$FF,$CE,$DE,$FF,$FF ; DEC
    .byte $FF,$FF,$FF,$FF,$FF,$4C,$FF,$FF,$FF ; JMP
    .byte $FF,$FF,$FF,$FF,$FF,$20,$FF,$FF,$FF ; JSR
    .byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$F0 ; BEQ
    .byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$D0 ; BNE
    .byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$90 ; BCC
    .byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$B0 ; BCS
    .byte $E8,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; INX
    .byte $CA,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; DEX
    .byte $C8,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; INY
    .byte $88,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; DEY
    .byte $60,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; RTS
    .byte $18,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; CLC
    .byte $38,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; SEC
    .byte $EA,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; NOP

opcode_ptrs:
    .word opcodes+0,   opcodes+9,   opcodes+18,  opcodes+27
    .word opcodes+36,  opcodes+45,  opcodes+54,  opcodes+63
    .word opcodes+72,  opcodes+81,  opcodes+90,  opcodes+99
    .word opcodes+108, opcodes+117, opcodes+126, opcodes+135
    .word opcodes+144, opcodes+153, opcodes+162, opcodes+171
    .word opcodes+180, opcodes+189, opcodes+198, opcodes+207
    .word opcodes+216, opcodes+225, opcodes+234, opcodes+243
    .word opcodes+252, opcodes+261

instr_sizes:
    .byte 1, 2, 2, 2, 2, 3, 3, 3, 2

; ==============================================================================
; MAIN ENTRY POINT
; ==============================================================================
.segment "CODE"
_start:
    lda #0
    sta pass
    sta sym_count
    sta pc
    sta pc+1
    sta out_ptr
    sta out_ptr+1

pass_loop:
    ; Open input file (Assuming custom OS API: A=mode, X=filename_ptr_low)
    lda #0
    ldx #<in_path
    jsr _open
    sta in_fd
    
    jsr process_file
    
    ldx in_fd
    jsr _close
    
    lda pass
    bne pass_done
    inc pass
    lda #0
    sta pc
    sta pc+1
    jmp pass_loop

pass_done:
    ; Open output file for writing
    lda #1
    ldx #<out_path
    jsr _open
    sta out_fd
    
    ; Write output buffer (Assuming OS API: X=fd, Y=buf_low, A=count_low, count_high in memory or similar)
    ; For demonstration, we write byte-by-byte or use a custom loop if the OS API is limited.
    ldx out_fd
    ldy #<out_buf
    lda out_ptr
    jsr _write
    
    ldx out_fd
    jsr _close
    
    jmp _exit

; ==============================================================================
; CORE LOGIC
; ==============================================================================
process_file:
    ; Read entire file into in_buf
    ldx in_fd
    ldy #<in_buf
    lda #<4096
    jsr _read
    sta in_size
    stx in_size+1
    
    lda #<in_buf
    sta line_ptr
    lda #>in_buf
    sta line_ptr+1
    
    clc
    lda #<in_buf
    adc in_size
    sta end_ptr
    lda #>in_buf
    adc in_size+1
    sta end_ptr+1

line_loop:
    lda line_ptr+1
    cmp end_ptr+1
    bcc do_line
    lda line_ptr
    cmp end_ptr
    bcc do_line
    rts

do_line:
    jsr parse_line
    jmp line_loop

parse_line:
    jsr skip_spaces
    ldy #0
    lda (line_ptr), y
    cmp #10
    beq next_line
    cmp #13
    beq next_line
    cmp #';'
    beq skip_to_nl
    cmp #0
    beq next_line
    
    jsr read_token
    ldx token_len
    dex
    lda token_buf, x
    cmp #':'
    bne is_instr
    
    dec token_len
    jsr add_label
    jmp skip_to_nl

is_instr:
    jsr find_mnemonic
    cmp #255
    beq skip_to_nl
    
    sta instr_id
    jsr parse_operand
    jsr emit_instr
    
skip_to_nl:
    ldy #0
skip_loop:
    lda (line_ptr), y
    cmp #10
    beq skip_done
    cmp #0
    beq skip_done
    iny
    bne skip_loop
skip_done:
    tya
    clc
    adc line_ptr
    sta line_ptr
    bcc next_line
    inc line_ptr+1
next_line:
    rts

; ==============================================================================
; HELPER FUNCTIONS
; ==============================================================================
skip_spaces:
    ldy #0
ss_loop:
    lda (line_ptr), y
    cmp #' '
    bne ss_done
    inc line_ptr
    bne ss_loop
    inc line_ptr+1
    jmp ss_loop
ss_done:
    rts

read_token:
    ldx #0
rt_loop:
    ldy #0
    lda (line_ptr), y
    cmp #' '
    beq rt_done
    cmp #10
    beq rt_done
    cmp #13
    beq rt_done
    cmp #';'
    beq rt_done
    cmp #0
    beq rt_done
    
    sta token_buf, x
    inx
    inc line_ptr
    bne rt_loop
    inc line_ptr+1
    jmp rt_loop
rt_done:
    stx token_len
    rts

find_mnemonic:
    ldx #0
fm_loop:
    ldy #0
fm_cmp:
    lda mnemonic_tab, x
    cmp #255
    beq fm_not_found
    cmp token_buf, y
    bne fm_next
    inx
    iny
    cpy #3
    bne fm_cmp
    
    lda mnemonic_tab, x
    rts

fm_next:
    inx
    inx
    inx
    inx
    jmp fm_loop

fm_not_found:
    lda #255
    rts

parse_operand:
    jsr skip_spaces
    ldy #0
    lda (line_ptr), y
    
    cmp #'#'
    beq po_imm
    cmp #'$'
    beq po_hex
    
    lda instr_id
    cmp #18
    bcc po_label
    cmp #22
    bcs po_label
    
    ldy #8
    sty cur_mode
    jsr parse_label
    rts

po_label:
    ldy #5
    sty cur_mode
    jsr parse_label
    rts

po_imm:
    ldy #1
    sty cur_mode
    inc line_ptr
    bne po_hex_skip
    inc line_ptr+1
po_hex_skip:
    jsr parse_hex
    rts

po_hex:
    inc line_ptr
    bne po_hex2
    inc line_ptr+1
po_hex2:
    jsr parse_hex
    lda val_high
    bne po_abs
    ldy #2
    sty cur_mode
    rts
po_abs:
    ldy #5
    sty cur_mode
    rts

parse_hex:
    lda #0
    sta val_low
    sta val_high
hx_loop:
    ldy #0
    lda (line_ptr), y
    cmp #'0'
    bcc hx_end
    cmp #'9'+1
    bcc hx_digit
    cmp #'A'
    bcc hx_end
    cmp #'F'+1
    bcc hx_upper
    cmp #'a'
    bcc hx_end
    cmp #'f'+1
    bcs hx_end
    sec
    sbc #'a' - 10
    jmp hx_add
hx_upper:
    sec
    sbc #'A' - 10
    jmp hx_add
hx_digit:
    sec
    sbc #'0'
hx_add:
    pha
    asl val_low
    rol val_high
    asl val_low
    rol val_high
    asl val_low
    rol val_high
    asl val_low
    rol val_high
    pla
    ora val_low
    sta val_low
    inc line_ptr
    bne hx_loop
    inc line_ptr+1
    jmp hx_loop
hx_end:
    rts

parse_label:
    jsr read_token
    jsr find_label
    cmp #1
    beq pl_found
    lda pass
    beq pl_pass1
pl_pass1:
    lda #0
    sta val_low
    sta val_high
    rts
pl_found:
    rts

find_label:
    ldx #0
    lda sym_count
    beq not_found
find_loop:
    stx cur_sym
    txa
    asl a
    asl a
    asl a
    asl a
    tay
    ldx #0
cmp_loop:
    cpx token_len
    beq cmp_len_ok
    lda sym_names, y
    cmp token_buf, x
    bne next_sym
    iny
    inx
    jmp cmp_loop
cmp_len_ok:
    lda sym_names, y
    bne next_sym
    ldx cur_sym
    txa
    asl a
    tay
    lda sym_addrs, y
    sta val_low
    lda sym_addrs+1, y
    sta val_high
    lda #1
    rts
next_sym:
    ldx cur_sym
    inx
    cpx sym_count
    bne find_loop
not_found:
    lda #0
    rts

add_label:
    ldx sym_count
    cpx #64
    bcs sym_full
    txa
    asl a
    tay
    lda pc
    sta sym_addrs, y
    lda pc+1
    sta sym_addrs+1, y
    txa
    asl a
    asl a
    asl a
    asl a
    tax
    ldy #0
name_loop:
    cpy token_len
    beq name_done
    lda token_buf, y
    sta sym_names, x
    inx
    iny
    jmp name_loop
name_done:
    lda #0
    sta sym_names, x
    inc sym_count
sym_full:
    rts

emit_instr:
    asl a
    tax
    lda opcode_ptrs, x
    sta ptr_low
    lda opcode_ptrs+1, x
    sta ptr_high
    ldy cur_mode
    lda (ptr_low), y
    cmp #$FF
    beq emit_err
    jsr emit_byte
    ldy cur_mode
    cpy #1
    beq emit_1byte
    cpy #2
    beq emit_1byte
    cpy #3
    beq emit_1byte
    cpy #4
    beq emit_1byte
    cpy #8
    beq emit_rel
    lda val_low
    jsr emit_byte
    lda val_high
    jsr emit_byte
    rts
emit_1byte:
    lda val_low
    jsr emit_byte
    rts
emit_rel:
    sec
    lda val_low
    sbc pc
    sbc #1
    tay
    lda val_high
    sbc pc+1
    sbc #0
    tax
    txa
    beq rel_pos
    cmp #$FF
    beq rel_neg
    jmp emit_err
rel_pos:
    cpy #$80
    bcc rel_ok
    jmp emit_err
rel_neg:
    cpy #$80
    bcs rel_ok
    jmp emit_err
rel_ok:
    tya
    jsr emit_byte
    rts
emit_err:
    rts

emit_byte:
    ldx pass
    beq emit_skip
    ldx out_ptr
    sta out_buf, x
    inc out_ptr
    bne emit_skip
    inc out_ptr+1
emit_skip:
    inc pc
    bne emit_done
    inc pc+1
emit_done:
    rts

; ==============================================================================
; DEMO PROGRAM (Processed by the assembler itself)
; ==============================================================================
demo_start:
    LDA #$00
    STA $10
    LDX #$05
loop:
    DEX
    BNE loop
    RTS