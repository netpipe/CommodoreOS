#include <stdint.h>

// Seed values
static uint16_t lfsr = 0xACE1u;     // 16-bit LFSR
static uint16_t xorshift = 0xC3D2u;  // 16-bit xorshift

// LFSR function
uint16_t lfsr_rand() {
    uint16_t lsb = lfsr & 1u;  // Get LSB (least significant bit)
    lfsr >>= 1;                // Shift register
    if (lsb) {                 // If the output bit is 1, apply toggle mask.
        lfsr ^= 0xB400u;
    }
    return lfsr;
}

// Xorshift function
uint16_t xorshift_rand() {
    xorshift ^= xorshift << 7;
    xorshift ^= xorshift >> 9;
    xorshift ^= xorshift << 8;
    return xorshift;
}

// Combined RNG function
uint16_t rand() {
    return lfsr_rand() ^ xorshift_rand();
}

int main() {
    // Seed the generators (if needed, you can modify the seeding mechanism)
    lfsr = 0xACE1u;
    xorshift = 0xC3D2u;

    uint16_t randomNumber = rand();
    // Use randomNumber for your purposes

    return 0;
}

