// demo.c
#include <stdio.h>
#include "floatlib.h"

int main(void) {
    cbm_float a, b, result;

    cbm_float_from_int(&a, 10);
    cbm_float_from_int(&b, 3);

    cbm_float_add(&result, &a, &b);
    printf("Addition result: %ld\n", cbm_float_to_int(&result));

    cbm_float_sub(&result, &a, &b);
    printf("Subtraction result: %ld\n", cbm_float_to_int(&result));

    cbm_float_mul(&result, &a, &b);
    printf("Multiplication result: %ld\n", cbm_float_to_int(&result));

    cbm_float_div(&result, &a, &b);
    printf("Division result: %ld\n", cbm_float_to_int(&result));

    return 0;
}
