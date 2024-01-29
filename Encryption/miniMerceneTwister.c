#include <stdint.h>
#include <stdio.h>
#define MT_SIZE 16  // Much smaller than the standard 624
#define MT_PERIOD 15

static uint16_t mt[MT_SIZE];
static uint16_t index = MT_SIZE + 1;

// Initialize the generator with a seed
void mt_init(uint16_t seed) {
uint16_t i ;
    mt[0] = seed;
    for ( i = 1; i < MT_SIZE; ++i) {
        mt[i] = (1812433253 * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i) & 0xFFFF;
    }
    index = MT_SIZE;
}

// Generate a pseudo-random number
uint16_t mt_rand() {
uint16_t i;
uint16_t y;
    if (index >= MT_SIZE) {
        if (index > MT_SIZE) {
            mt_init(5489); // Default initialization with a fixed seed
        }

        // Twist
        for ( i = 0; i < MT_SIZE; ++i) {
             y = (mt[i] & 0x8000) + (mt[(i + 1) % MT_SIZE] & 0x7FFF);
            mt[i] = mt[(i + MT_PERIOD) % MT_SIZE] ^ (y >> 1);
            if (y % 2 != 0) { // y is odd
                mt[i] ^= 25673; // A reduced constant from the full MT
            }
        }
        index = 0;
    }

     y = mt[index++];
    y ^= (y >> 11);
    y ^= (y << 7) & 2636928640;
    y ^= (y << 15) & 4022730752;
    y ^= (y >> 18);
    return y;
}

int main() {
uint16_t randomNumber;
    mt_init(1234); // Seed with a number
     randomNumber = mt_rand();
    // Use randomNumber for your purposes
    printf ("random number %u",randomNumber);
    return 0;
}

