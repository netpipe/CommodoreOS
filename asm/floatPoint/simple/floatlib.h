// floatlib.h
#ifndef FLOATLIB_H
#define FLOATLIB_H

#include <stdint.h>

// Define the CBM float type as a 5-byte array
typedef struct {
    uint8_t bytes[5];
} cbm_float;

// Function prototypes for basic operations
void cbm_float_from_int(cbm_float *result, int32_t value);
int32_t cbm_float_to_int(const cbm_float *value);
void cbm_float_add(cbm_float *result, const cbm_float *a, const cbm_float *b);
void cbm_float_sub(cbm_float *result, const cbm_float *a, const cbm_float *b);
void cbm_float_mul(cbm_float *result, const cbm_float *a, const cbm_float *b);
void cbm_float_div(cbm_float *result, const cbm_float *a, const cbm_float *b);

// Additional functions can be added as needed

#endif // FLOATLIB_H
