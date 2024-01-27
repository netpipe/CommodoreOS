#include <stdio.h>
#include <stdlib.h>

#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_MASK 0xFFFF
#define MAX_SINE_VALUE 65535
#define TABLE_SIZE 91  // Lookup table for 0 to 90 degrees

#define DEC_BASE 10000

#define TO_FIXED_POINT(x) ((x) << FIXED_POINT_SHIFT)
#define FROM_FIXED_POINT(x) ((x) >> FIXED_POINT_SHIFT)


static const int sine_table[TABLE_SIZE] = {
0, 1143, 2287, 3429, 4571, 5711, 6850, 7986, 9120, 10251, 
11380, 12504, 13625, 14742, 15854, 16961, 18063, 19160, 20251, 21336, 
22414, 23485, 24549, 25606, 26655, 27696, 28728, 29752, 30766, 31771, 
32767, 33753, 34728, 35692, 36646, 37589, 38520, 39439, 40347, 41242, 
42125, 42994, 43851, 44694, 45524, 46340, 47141, 47929, 48701, 49459, 
50202, 50930, 51642, 52338, 53018, 53683, 54330, 54962, 55576, 56174, 
56754, 57318, 57863, 58392, 58902, 59394, 59869, 60325, 60762, 61182, 
61582, 61964, 62327, 62671, 62996, 63301, 63588, 63855, 64102, 64330, 
64539, 64728, 64897, 65046, 65175, 65285, 65375, 65445, 65495, 65525, 
65535
};


int from_fixed_point2(int value) {
    unsigned long copy_value = value;
    copy_value = copy_value * DEC_BASE;
    return (int)((copy_value / FIXED_POINT_MASK) & FIXED_POINT_MASK);
}


// Fixed-point sine function for 0-360 degrees
int fixed_sin(int angle) {
 int quadrant;
    angle = FROM_FIXED_POINT(angle) % 360; // Convert to degrees and modulo 360
    quadrant = (angle / 90) % 4;
    angle = angle % 90;
// add a specifier or char to show quadrant ?
    if (quadrant == 0) return sine_table[angle];
    if (quadrant == 1) return sine_table[90 - angle];
    if (quadrant == 2) return -sine_table[angle];
    return -sine_table[90 - angle];
}

int fixed_sin2(int angle) {
int quadrant;
int sine_val;
    angle = FROM_FIXED_POINT(angle) % 360; // Convert from fixed-point and normalize angle
    // Quadrant determination
    quadrant = (angle / 90) % 4;
    angle = angle % 90;
    if (quadrant == 1 || quadrant == 3) {
        angle = 90 - angle;
    }
    sine_val = sine_table[angle];
    if (quadrant == 2 || quadrant == 3) {
        sine_val = MAX_SINE_VALUE - sine_val; // Sine is negative in the 3rd and 4th quadrants
    }
    return quadrant > 1 ? -sine_val : sine_val;
}

// Fixed-point cosine function
int fixed_cos(int angle) {
    return fixed_sin((90 << FIXED_POINT_SHIFT) - angle);
}

// Fixed-point tangent function (simplified and non-robust)
int fixed_tan(int angle) {
    int sin_val = fixed_sin(angle);
    int cos_val = fixed_cos(angle);
    if (cos_val == 0) return 0; // Simplified handling of undefined values
    return (sin_val << FIXED_POINT_SHIFT) / cos_val;
}

int find_angle_for_sine(int sine_value) {

    int angle= MAX_SINE_VALUE;
    int diff= MAX_SINE_VALUE;
    int min_diff = 0;
    int closest_angle = 0;
    printf("Finding angle for sine value: %d\n", sine_value);

    // Search in the first quadrant (0 to 90 degrees)
    for (angle = 0; angle < (TABLE_SIZE); angle++) {
        diff = abs(sine_table[angle] - sine_value);
   //     printf("Angle: %d, Sine: %d, Diff: %d\n", angle, sine_table[angle], diff);

        if (diff <= min_diff) {
            min_diff = diff;
            closest_angle = angle;
    printf("Final closest angle: %d degrees\n", closest_angle);
    return closest_angle;
        }
    }

    // Debugging output
   // printf("Closest angle in first quadrant: %d degrees, Min diff: %d\n", closest_angle, min_diff);

    // Check for the second quadrant (90 to 180 degrees)
    //for (angle = TABLE_SIZE - 1; angle >= 0; angle--) {
    //    diff = abs(sine_table[angle] - sine_value);
    //    printf("Angle: %d, Sine: %d, Diff: %d\n", 180 - angle, sine_table[angle], diff);

    //    if (diff < min_diff) {
   //         min_diff = diff;
   //         closest_angle = 180 - angle;
     //           printf("Final closest angle: %d degrees\n", closest_angle);
   // return closest_angle;
   //     }
   // }

   // printf("Final closest angle: %d degrees\n", closest_angle);
   // return closest_angle;
}

int fixed_arcsin(unsigned int sine_value)
{
int i;
//printf("fixed_arcsin %u",sine_value);
//sine_value = (unsigned int)sine_value; //was testing this not for use
    // Handle edge cases
   // if (sine_value <= 0) return 0;
    //if (sine_value <= 0) sine_value= sine_value+27144;
    if (sine_value >= MAX_SINE_VALUE) return TO_FIXED_POINT(90);
//halfway checker to see weather it rounds up or down

//printf("test %u\n",sine_value);
    // Search the table
    for (i = 0; i < TABLE_SIZE; i++) {
        if (sine_table[i] == sine_value) {
            printf("found sine at %i\n",i);
            return i;
        }
        else if (sine_table[i] > sine_value) {
            // skip linear interpolation for better approximation
            // because current fixed point has [0..1] limit
             printf("found sine_value3 at %u,%i\n",sine_value,i);            
            return i;
        }
    }

    return  TO_FIXED_POINT(90); // Should not reach here

}

unsigned long pow_int(unsigned int base, unsigned int exp) {
    unsigned long result = 1;

    while (exp) {
        if (exp & 1) {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }

    return result;
}

unsigned int sqrt_fixed(unsigned int x) {
    unsigned int result = 0;
    unsigned int bit = 1 << (FIXED_POINT_SHIFT - 1);

    while (bit > x)
        bit >>= 2;

    while (bit != 0) {
        unsigned int temp = result + bit;
        if (x >= temp) {
            x -= temp;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }

        bit >>= 2;
    }

    return result;
}


int main() {
    int angle = TO_FIXED_POINT(45); // 45 degrees in fixed-point format
    int angle2 = fixed_sin(TO_FIXED_POINT(120));
    printf("Fixed-point sine of 120 degrees: %d (raw value)\n", fixed_sin(TO_FIXED_POINT(120)));
        printf("Fixed-point sine of 120 degrees: %d.%04u\n", from_fixed_point2(angle2) / DEC_BASE,      from_fixed_point2(angle2) % DEC_BASE);
        
    printf("Fixed-point sine of 45 degrees: %d (raw value)\n", fixed_sin(angle));

    printf("Fixed-point sine of 45 degrees: %d.%04u\n", from_fixed_point2(fixed_sin(angle))/DEC_BASE, from_fixed_point2(fixed_sin(angle)) % DEC_BASE);
    
    printf("Fixed-point sine of 10 degrees: %d.%04u\n", from_fixed_point2(fixed_sin(10 << FIXED_POINT_SHIFT))/ DEC_BASE, from_fixed_point2(fixed_sin(10 << FIXED_POINT_SHIFT)) % DEC_BASE);
    printf("Fixed-point sine of 90 degrees: %d.%04u\n", from_fixed_point2(fixed_sin(TO_FIXED_POINT(90))) / DEC_BASE, from_fixed_point2(fixed_sin(TO_FIXED_POINT(90))) % DEC_BASE);
  //  printf("Fixed-point sine of 360 degrees: %d.%04u\n", from_fixed_point2(fixed_sin(360 << FIXED_POINT_SHIFT)) / DEC_BASE, from_fixed_point2(fixed_sin(360 << FIXED_POINT_SHIFT)) % DEC_BASE);
 //   printf("Fixed-point sine of 180 degrees: %d.%04u\n", from_fixed_point2(fixed_sin(180 << FIXED_POINT_SHIFT)) / DEC_BASE, from_fixed_point2(fixed_sin(180 << FIXED_POINT_SHIFT)) % DEC_BASE);
      printf("fixed_sin: %04u\n",   fixed_sin((unsigned)angle));
    printf("fixed_arcsin: %d\n", fixed_arcsin(fixed_sin(angle)));
    printf("fixed_arcsin22: %d  \n", fixed_arcsin(1100));
        printf("fixed_arcsin23: %d  \n", fixed_arcsin(3429));
          printf("fixed_arcsin24: %d  \n", fixed_arcsin(4501));
        
   printf("find_angle_for_sine: %d  \n", find_angle_for_sine(fixed_sin(angle)));
     //printf("find_angle_for_sine : %i  \n", FROM_FIXED_POINT(fixed_sin(angle)) / MAX_SINE_VALUE);
    printf("find_angle_for_sine2: %d  \n", find_angle_for_sine(1100));
    
   //     printf("Fixed-point sine of 45 degrees converted backfromfixed: %d  \n", FROM_FIXED_POINT(fixed_sin(angle)));
  //      printf("Fixed-point sine of 45 degrees converted back: %i  \n", FROM_FIXED_POINT(TO_FIXED_POINT(45)));
 
    
                    //       TO_FIXED_POINT(x)
    printf("Fixed-point cosine of 45 degrees: %d.%04u\n", from_fixed_point2(fixed_cos(angle)) / DEC_BASE, from_fixed_point2(fixed_cos(angle)) % DEC_BASE);
    printf("Fixed-point tangent of 45 degrees: %d.%04u\n", from_fixed_point2(fixed_tan(angle)) / DEC_BASE, from_fixed_point2(fixed_tan(angle)) % DEC_BASE);
    
  
   // printf("Angle for the sine value ~0.5: %d degrees\n", find_angle_for_sine(TO_FIXED_POINT(0.5)));
    return 0;
}

