#include <c64.h>
#include <conio.h>
#include <stdint.h>
#include <stdio.h>

// C64 ROM floats are exactly 5 bytes: [exponent, m1, m2, m3, sign]
// We must use this format to talk to the BASIC ROM routines.
typedef unsigned char c64_float_t[5];

// Global pointers to safely pass addresses to cc65 inline assembly
static unsigned int ptr_a;
static unsigned int ptr_b;
static unsigned int ptr_res;
static int16_t val_in;
static int16_t val_out;

// --- ROM Addresses ---
#define MOVFM  0xBBA2  // Load memory (A=lo, Y=hi) to FAC
#define MOVMF  0xBBD4  // Store FAC to memory (X=lo, Y=hi)
#define FADD   0xB867  // Add memory (A=lo, Y=hi) to FAC
#define FSUB   0xB850  // Subtract FAC from memory (A=lo, Y=hi) -> Result: RAM - FAC
#define FMULT  0xBA28  // Multiply memory (A=lo, Y=hi) by FAC
#define FDIV   0xBB0F  // Divide memory (A=lo, Y=hi) by FAC -> Result: RAM / FAC
#define GIVAYF 0xB391  // Convert 16-bit signed int (A=hi, Y=lo) to float in FAC
#define FACINX 0xB1AA  // Convert float in FAC to 16-bit signed int (A=hi, Y=lo)

// Convert 16-bit integer to C64 ROM float
void int_to_float(int16_t val, c64_float_t *res) {
    ptr_res = (unsigned int)res;
    val_in = val;
    
    // GIVAYF ($B391) expects A=high byte, Y=low byte of the 16-bit int
    __asm__ ("lda val_in+1"); 
    __asm__ ("ldy val_in");   
    __asm__ ("jsr $B391");    // Convert to float in FAC
    
    // MOVMF ($BBD4) stores FAC to memory (X=low addr, Y=high addr)
    __asm__ ("ldx ptr_res");
    __asm__ ("ldy ptr_res+1");
    __asm__ ("jsr $BBD4");    
}

// Convert C64 ROM float back to 16-bit integer
void float_to_int(c64_float_t *a) {
    ptr_a = (unsigned int)a;
    
    // MOVFM ($BBA2) loads memory (A=low addr, Y=high addr) to FAC
    __asm__ ("lda ptr_a");
    __asm__ ("ldy ptr_a+1");
    __asm__ ("jsr $BBA2");    
    
    // FACINX ($B1AA) converts FAC to 16-bit int (A=high byte, Y=low byte)
    __asm__ ("jsr $B1AA");    
    
    // Store the result back to C variable (little-endian)
    __asm__ ("sty val_out");   // low byte
    __asm__ ("sta val_out+1"); // high byte
}

// Addition: res = a + b
void c64_add(c64_float_t *a, c64_float_t *b, c64_float_t *res) {
    ptr_a = (unsigned int)a;
    ptr_b = (unsigned int)b;
    ptr_res = (unsigned int)res;
    
    __asm__ ("lda ptr_b");
    __asm__ ("ldy ptr_b+1");
    __asm__ ("jsr $BBA2");    // Load b into FAC
    
    __asm__ ("lda ptr_a");
    __asm__ ("ldy ptr_a+1");
    __asm__ ("jsr $B867");    // FADD: Add a to FAC
    
    __asm__ ("ldx ptr_res");
    __asm__ ("ldy ptr_res+1");
    __asm__ ("jsr $BBD4");    // Store FAC to res
}

// Subtraction: res = a - b
void c64_sub(c64_float_t *a, c64_float_t *b, c64_float_t *res) {
    ptr_a = (unsigned int)a;
    ptr_b = (unsigned int)b;
    ptr_res = (unsigned int)res;
    
    __asm__ ("lda ptr_b");
    __asm__ ("ldy ptr_b+1");
    __asm__ ("jsr $BBA2");    // Load b into FAC
    
    // FSUB ($B850) does Memory - FAC. We pass 'a', so it calculates a - b.
    __asm__ ("lda ptr_a");
    __asm__ ("ldy ptr_a+1");
    __asm__ ("jsr $B850");    
    
    __asm__ ("ldx ptr_res");
    __asm__ ("ldy ptr_res+1");
    __asm__ ("jsr $BBD4");    
}

// Multiplication: res = a * b
void c64_mul(c64_float_t *a, c64_float_t *b, c64_float_t *res) {
    ptr_a = (unsigned int)a;
    ptr_b = (unsigned int)b;
    ptr_res = (unsigned int)res;
    
    __asm__ ("lda ptr_b");
    __asm__ ("ldy ptr_b+1");
    __asm__ ("jsr $BBA2");    // Load b into FAC
    
    __asm__ ("lda ptr_a");
    __asm__ ("ldy ptr_a+1");
    __asm__ ("jsr $BA28");    // FMULT: Multiply a by FAC
    
    __asm__ ("ldx ptr_res");
    __asm__ ("ldy ptr_res+1");
    __asm__ ("jsr $BBD4");    
}

// Division: res = a / b
void c64_div(c64_float_t *a, c64_float_t *b, c64_float_t *res) {
    ptr_a = (unsigned int)a;
    ptr_b = (unsigned int)b;
    ptr_res = (unsigned int)res;
    
    __asm__ ("lda ptr_b");
    __asm__ ("ldy ptr_b+1");
    __asm__ ("jsr $BBA2");    // Load b into FAC
    
    // FDIV ($BB0F) does Memory / FAC. We pass 'a', so it calculates a / b.
    __asm__ ("lda ptr_a");
    __asm__ ("ldy ptr_a+1");
    __asm__ ("jsr $BB0F");    
    
    __asm__ ("ldx ptr_res");
    __asm__ ("ldy ptr_res+1");
    __asm__ ("jsr $BBD4");    
}

int main(void) {
    c64_float_t f_a, f_b, f_res;
    
    clrscr();
    cprintf("C64 ROM Float Math Demo\r\n\r\n");
    
    // 1. Addition: 500 + 300 = 800
    int_to_float(500, &f_a);
    int_to_float(300, &f_b);
    c64_add(&f_a, &f_b, &f_res);
    float_to_int(&f_res);
    cprintf("500 + 300 = %d\r\n", val_out);
    
    // 2. Multiplication: 12 * 34 = 408
    int_to_float(12, &f_a);
    int_to_float(34, &f_b);
    c64_mul(&f_a, &f_b, &f_res);
    float_to_int(&f_res);
    cprintf("12 * 34   = %d\r\n", val_out);
    
    // 3. Subtraction: 100 - 45 = 55
    int_to_float(100, &f_a);
    int_to_float(45, &f_b);
    c64_sub(&f_a, &f_b, &f_res);
    float_to_int(&f_res);
    cprintf("100 - 45  = %d\r\n", val_out);

    // 4. Division: 144 / 12 = 12
    int_to_float(144, &f_a);
    int_to_float(12, &f_b);
    c64_div(&f_a, &f_b, &f_res);
    float_to_int(&f_res);
    cprintf("144 / 12  = %d\r\n", val_out);
    
    // 5. Negative Math: -5 * 6 = -30
    int_to_float(-5, &f_a);
    int_to_float(6, &f_b);
    c64_mul(&f_a, &f_b, &f_res);
    float_to_int(&f_res);
    cprintf("-5 * 6    = %d\r\n", val_out);

    cprintf("\r\nPress any key to exit...");
    cgetc();
    return 0;
}