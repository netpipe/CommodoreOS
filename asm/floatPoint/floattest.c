// fastfloat.c - Single-file minimal float lib for C64 (cc65)
// Emulates basic IEEE 754 float ops using uint32_t, with real KERNAL support

#ifndef __INT32_T_DEFINED__
#define __INT32_T_DEFINED__
typedef signed long int32_t;
#endif

#include <stdint.h>
#include <stdio.h>

// Basic IEEE 754 single-precision float type
typedef uint32_t f32;

// Write a float to C64 memory (little endian)
void write_float(uint16_t addr, f32 val) {
    *(uint8_t*)(addr)     = (uint8_t)(val & 0xFF);
    *(uint8_t*)(addr + 1) = (uint8_t)((val >> 8) & 0xFF);
    *(uint8_t*)(addr + 2) = (uint8_t)((val >> 16) & 0xFF);
    *(uint8_t*)(addr + 3) = (uint8_t)((val >> 24) & 0xFF);
    *(uint8_t*)(addr + 4) = 0; // high byte of mantissa (unused in IEEE)
}

// Read a float from C64 memory (little endian)
f32 read_float(uint16_t addr) {
    f32 result = 0;
    result |= *(uint8_t*)(addr);
    result |= ((uint32_t)*(uint8_t*)(addr + 1)) << 8;
    result |= ((uint32_t)*(uint8_t*)(addr + 2)) << 16;
    result |= ((uint32_t)*(uint8_t*)(addr + 3)) << 24;
    return result;
}

// Convert int to float using ROM routine ($D4F9)
f32 int_to_f32(int32_t val) {
    *(uint8_t*)0x14 = (uint8_t)(val & 0xFF);
    *(uint8_t*)0x15 = (uint8_t)((val >> 8) & 0xFF);
    *(uint8_t*)0x16 = (uint8_t)((val >> 16) & 0xFF);
    *(uint8_t*)0x17 = (uint8_t)((val >> 24) & 0xFF);
    __asm__("JSR $D4F9"); // Convert integer in $14-$17 to FAC
    return read_float(0x61);
}

// Convert FAC to int using ROM routine ($BC0C)
int32_t f32_to_int(f32 val) {
    write_float(0x61, val);
    __asm__("JSR $BC0C"); // Convert FAC to integer in $14-$17
    int32_t res = 0;
    res |= *(uint8_t*)0x14;
    res |= ((uint32_t)*(uint8_t*)0x15) << 8;
    res |= ((uint32_t)*(uint8_t*)0x16) << 16;
    res |= ((uint32_t)*(uint8_t*)0x17) << 24;
    return res;
}

// Real float addition using KERNAL ROM
f32 f32_add(f32 a, f32 b) {
    write_float(0x66, a); // ARG
    write_float(0x61, b); // FAC
    __asm__("JSR $BBA2"); // FAC = ARG + FAC
    return read_float(0x61);
}

// Real float multiplication using KERNAL ROM
f32 f32_mul(f32 a, f32 b) {
    write_float(0x66, a); // ARG
    write_float(0x61, b); // FAC
    __asm__("JSR $BBD4"); // FAC = ARG * FAC
    return read_float(0x61);
}

// Real float division using KERNAL ROM
f32 f32_div(f32 a, f32 b) {
    write_float(0x66, a); // ARG
    write_float(0x61, b); // FAC
    __asm__("JSR $BBFC"); // FAC = ARG / FAC
    return read_float(0x61);
}

// Real float subtraction using KERNAL ROM
f32 f32_sub(f32 a, f32 b) {
    write_float(0x61, b);
    __asm__("JSR $BDB5"); // FAC = -FAC
    f32 neg_b_val = read_float(0x61);
    return f32_add(a, neg_b_val);
}

// Negate float using ROM FNEG
f32 f32_neg(f32 val) {
    write_float(0x61, val);
    __asm__("JSR $BDB5"); // FAC = -FAC
    return read_float(0x61);
}

// Square root using ROM FSQR ($BDA7)
f32 f32_sqrt(f32 val) {
    write_float(0x61, val);
    __asm__("JSR $BDA7"); // FAC = sqrt(FAC)
    return read_float(0x61);
}

// Absolute value using ROM ABS ($BDCB)
f32 f32_abs(f32 val) {
    write_float(0x61, val);
    __asm__("JSR $BDCB"); // FAC = |FAC|
    return read_float(0x61);
}

// Demo usage
int main(void) {
    f32 a = int_to_f32(5);
    f32 b = int_to_f32(3);

    f32 sum = f32_add(a, b);
    f32 diff = f32_sub(a, b);
    f32 prod = f32_mul(a, b);
    f32 quot = f32_div(a, b);
    f32 neg = f32_neg(b);
    f32 sqr = f32_sqrt(a);
    f32 abv = f32_abs(neg);

    printf("a + b = %ld\n", f32_to_int(sum));
    printf("a - b = %ld\n", f32_to_int(diff));
    printf("a * b = %ld\n", f32_to_int(prod));
    printf("a / b = %ld\n", f32_to_int(quot));
    printf("-b    = %ld\n", f32_to_int(neg));
    printf("sqrt(a) = %ld\n", f32_to_int(sqr));
    printf("abs(-b) = %ld\n", f32_to_int(abv));

    return 0;
}
