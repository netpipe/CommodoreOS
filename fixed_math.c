#include <c64.h>
#include <conio.h>
#include <stdint.h>
#include <stdio.h>

// ============================================================================
// CORE TYPES AND CONSTANTS
// ============================================================================

// 16.16 Fixed Point format: 
// Top 16 bits = Integer part, Bottom 16 bits = Fractional part
// Example: 1.5 = 0x00018000 (98304 in decimal)
typedef int32_t fixed_t;

#define FIXED_SHIFT 16
#define FIXED_ONE   (1L << FIXED_SHIFT) // 65536

// Conversion Macros
#define INT_TO_FIXED(x) ((fixed_t)(x) << FIXED_SHIFT)
#define FIXED_TO_INT(x) ((int)((x) >> FIXED_SHIFT))

// ============================================================================
// MATH OPERATIONS (Optimized for cc65's 32-bit architecture)
// ============================================================================

// Addition and Subtraction work natively with two's complement 16.16 format
#define fixed_add(a, b) ((a) + (b))
#define fixed_sub(a, b) ((a) - (b))

// Multiplication: (a * b) >> 16
// Because cc65 lacks 64-bit integers, we split the 32-bit variables into 
// 16-bit halves to prevent silent overflow during the intermediate calculation.
fixed_t fixed_mul(fixed_t a, fixed_t b) {
    int16_t a_hi = a >> 16;
    uint16_t a_lo = a & 0xFFFF;
    int16_t b_hi = b >> 16;
    uint16_t b_lo = b & 0xFFFF;

    // Calculate partial products
    uint32_t term1 = (uint32_t)(a_hi * b_hi) << 16;
    uint32_t term2 = (uint32_t)(a_hi * b_lo);
    uint32_t term3 = (uint32_t)(a_lo * b_hi);
    uint32_t term4 = ((uint32_t)(a_lo * b_lo)) >> 16;

    // Summing as uint32_t guarantees proper 32-bit two's complement wrap-around
    uint32_t sum = term1 + term2 + term3 + term4;
    return (fixed_t)sum;
}

// Division: (a / b) << 16
// Uses a bitwise long-division approach to calculate the fractional bits 
// without overflowing the 32-bit limit.
fixed_t fixed_div(fixed_t a, fixed_t b) {
    if (b == 0) return (a < 0) ? -2147483647 : 2147483647; // Prevent Div/0
    
    uint32_t abs_a = a < 0 ? -(uint32_t)a : (uint32_t)a;
    uint32_t abs_b = b < 0 ? -(uint32_t)b : (uint32_t)b;
    int sign = (a < 0) ^ (b < 0) ? -1 : 1;

    uint32_t int_part = abs_a / abs_b;
    uint32_t rem = abs_a % abs_b;
    uint32_t frac_part = 0;

    // Calculate the 16 fractional bits iteratively
    for (int i = 15; i >= 0; i--) {
        rem <<= 1;
        if (rem >= abs_b) {
            frac_part |= (1UL << i);
            rem -= abs_b;
        }
    }

    uint32_t final_res = (int_part << 16) | frac_part;
    return sign < 0 ? -(fixed_t)final_res : (fixed_t)final_res;
}

// ============================================================================
// TRIGONOMETRY (Sine/Cosine/Tangent)
// ============================================================================

// 91-point lookup table for 0 to 90 degrees.
// Values are scaled to 16.16 fixed point (e.g., 65536 = 1.0)
static const uint16_t sin_table[91] = {
    0, 1144, 2287, 3430, 4572, 5712, 6850, 7987, 9121, 10253,
    11381, 12506, 13626, 14743, 15855, 16962, 18064, 19161, 20252, 21337,
    22415, 23486, 24550, 25607, 26656, 27697, 28729, 29753, 30767, 31772,
    32768, 33754, 34729, 35693, 36647, 37590, 38521, 39440, 40348, 41243,
    42126, 42995, 43852, 44695, 45525, 46341, 47142, 47930, 48702, 49460,
    50203, 50931, 51643, 52339, 53019, 53684, 54331, 54963, 55577, 56175,
    56755, 57319, 57864, 58393, 58903, 59395, 59870, 60326, 60763, 61183,
    61583, 61965, 62328, 62672, 62997, 63302, 63589, 63856, 64103, 64331,
    64540, 64729, 64898, 65047, 65176, 65286, 65376, 65446, 65496, 65526, 65536
};

// Sine function (input in degrees, scaled to fixed-point)
fixed_t fixed_sin_deg(fixed_t angle) {
    // Normalize to 0-359 degrees
    int deg = FIXED_TO_INT(angle) % 360;
    if (deg < 0) deg += 360;

    int quadrant = deg / 90;
    int index = deg % 90;

    uint16_t val;
    if (quadrant == 0) {
        val = sin_table[index];
    } else if (quadrant == 1) {
        val = sin_table[90 - index];
    } else if (quadrant == 2) {
        val = sin_table[index];
        return -(fixed_t)val;
    } else {
        val = sin_table[90 - index];
        return -(fixed_t)val;
    }
    return (fixed_t)val;
}

fixed_t fixed_cos_deg(fixed_t angle) {
    return fixed_sin_deg(angle + INT_TO_FIXED(90));
}

fixed_t fixed_tan_deg(fixed_t angle) {
    fixed_t s = fixed_sin_deg(angle);
    fixed_t c = fixed_cos_deg(angle);
    if (c == 0) return (s > 0) ? 2147483647 : -2147483647;
    return fixed_div(s, c);
}

// ============================================================================
// DEMO MAIN
// ============================================================================
int main(void) {
    clrscr();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);
    textcolor(COLOR_WHITE);
    
    cprintf("CommodoreOS Fixed-Point Engine\r\n");
    cprintf("--------------------------------\r\n\r\n");

    // 1. Basic Arithmetic
    fixed_t a = INT_TO_FIXED(12);
    fixed_t b = INT_TO_FIXED(5);
    
    cprintf("12 + 5 = %d\r\n", FIXED_TO_INT(fixed_add(a, b)));
    cprintf("12 * 5 = %d\r\n", FIXED_TO_INT(fixed_mul(a, b)));
    cprintf("12 / 5 = %d\r\n", FIXED_TO_INT(fixed_div(a, b))); // Should be 2

    // 2. Fractional Math (The true test of fixed-point)
    fixed_t f1 = INT_TO_FIXED(2) + (INT_TO_FIXED(1) >> 1); // 2.5
    fixed_t f2 = INT_TO_FIXED(1) + (INT_TO_FIXED(1) >> 2); // 1.25
    
    cprintf("\r\n2.5 * 1.25 = ");
    fixed_t prod = fixed_mul(f1, f2);
    cprintf("%d.%d\r\n", FIXED_TO_INT(prod), (int)((prod & 0xFFFF) * 100) / 65536); // 3.125
    
    cprintf("2.5 / 1.25 = %d\r\n", FIXED_TO_INT(fixed_div(f1, f2))); // Should be 2

    // 3. Trigonometry
    cprintf("\r\nsin(30) = ");
    fixed_t s30 = fixed_sin_deg(INT_TO_FIXED(30));
    cprintf("%d.%04d\r\n", FIXED_TO_INT(s30), (int)((s30 & 0xFFFF) * 10000) / 65536); // ~0.5000

    cprintf("sin(90) = %d\r\n", FIXED_TO_INT(fixed_sin_deg(INT_TO_FIXED(90)))); // Should be 1
    cprintf("sin(270)= %d\r\n", FIXED_TO_INT(fixed_sin_deg(INT_TO_FIXED(270)))); // Should be -1

    cprintf("\r\nPress any key...");
    cgetc();
    return 0;
}