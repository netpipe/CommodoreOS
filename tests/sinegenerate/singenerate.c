#include <stdio.h>
#include <math.h>

#define TABLE_SIZE 91  // Angles from 0 to 90 degrees
#define MAX_SINE_VALUE 65535

int main() {
    for (int angle = 0; angle < TABLE_SIZE; ++angle) {
        double radian = angle * M_PI / 180.0;
        unsigned int fixed_point_sine = (unsigned int)(sin(radian) * MAX_SINE_VALUE);
        printf("%u, ", fixed_point_sine);

        if ((angle + 1) % 10 == 0) {
            printf("\n");
        }
    }
    return 0;
}
