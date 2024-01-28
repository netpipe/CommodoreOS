#include <stdio.h>

static const long factorials[] = {
/* 0!  */	        1,
/* 1!  */	        1,
/* 2!  */	        2,
/* 3!  */	        6,
/* 4!  */	       24,
/* 5!  */	      120,
/* 6!  */	      720,
/* 7!  */	     5040,
/* 8!  */	    40320,
/* 9!  */	   362880,
/* 10! */	  3628800,
/* 11! */	 39916800,
/* 12! */	479001600
};

static const unsigned int frac_points[] = {
	50000,
	25000,
	12500,
	 6250,
	 3125,
	 1563,  // 1562.5
	  781,  //  781.25
	  391,  //  390.625
	  195,  //  195.3125
	   98,  //  97.65625
	   49,  //  48.828125
	   24,  //  24.4140625
	   12,  //  12.20703125
		6,  //   6.103515625
		3,  //   3.0517578125
		2,  //   1.52587890625
};

typedef long fixed_float;

#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_MASK 0xFFFF


fixed_float int_to_float(int value)
{
	return value << FIXED_POINT_SHIFT;
}

int float_to_int(fixed_float value)
{
	return (value >> FIXED_POINT_SHIFT) & FIXED_POINT_MASK;
}

int get_int_part(fixed_float value)
{
	return (value >> FIXED_POINT_SHIFT) & FIXED_POINT_MASK;
}

unsigned int get_frac_part(fixed_float value)
{
	int i = 0;
	unsigned long accumulated = 0;
	unsigned int mask = 0x8000;
	while ( i < FIXED_POINT_SHIFT && mask != 0)
	{
		if (value & mask)
			accumulated += frac_points[i];
		i = i + 1;
		mask = mask >> 1;
	}
	return ((accumulated) / 10) & 0xFFFF;
}

fixed_float multiply_fixed_point(fixed_float multiplier1, fixed_float multiplier2)
{
	int i;
	int a;
	fixed_float product;
	fixed_float product_parts[4] = { 0 };
	if (multiplier1 < 0)
		multiplier1 ^= 0x80000000;
	if (multiplier2 < 0)
		multiplier2 ^= 0x80000000;


	
	for (i = 0; i < 4; i++)
	{
		a = (multiplier2 >> (8 * i)) & 0xFF;
		product_parts[i] = multiplier1 * ((multiplier2 >> (8 * i)) & 0xFF);
	}

	if (product_parts[3] != 0)
	{
		printf("Math error: Overflow!\n");
	}

	product = product_parts[2] + ((unsigned long)product_parts[1] >> 8) + ((unsigned long)product_parts[0] >> 16);	
	return product;
}


fixed_float divide_fixed_point(fixed_float devidend, fixed_float divisor)
{
	fixed_float fraction = 0;
	int left_shifts = 0;
	int loops = 0;
	int x;
    int x21;
    
	if (divisor!=0)
	{
		if (devidend < 0)
			devidend ^= 0x80000000;
		if (divisor < 0)
			divisor ^= 0x80000000;
		
		while (devidend > divisor)
		{			
			divisor <<= 1;
			left_shifts++;
		}

		x = 1;

		while (devidend != 0 && divisor!=0)
		{
			loops++;
			fraction <<= 1;
			if (devidend > divisor)
			{
				fraction |= 1;
				devidend = devidend - divisor;
				//devidend = devidend >> 1;
			}
			//printf(".... Fraction: 0x%08X\n", fraction);
			divisor >>= 1;
		}

		if ((loops - 16 - left_shifts - 1) >= 0)
			fraction >>= (loops - 16 - left_shifts - 1);
		else
			fraction <<= (16 + left_shifts + 1 - loops);
		x21 = 1;

	}
	else
	{
		printf("Math error: Division by zero!\n");
	}

	return fraction;
}


int main()
{
	fixed_float angle1;
	fixed_float angle2;

	fixed_float val3;
	fixed_float val4;
fixed_float vv;
	fixed_float product;
	fixed_float fraction;

	angle1 = int_to_float(45); // 45 degrees in fixed-point format
	printf("Fixed point value %d.%04u\n", get_int_part(angle1), get_frac_part(angle1));

	angle2 = 0x002DC3C0; // 45.76465
	printf("Fixed point value %d.%04u\n", get_int_part(angle2), get_frac_part(angle2));

	product = multiply_fixed_point(angle1, angle2); // 2059.40925

	printf("45 * 45.76465 = Fixed point value %d.%04u\n", get_int_part(product), get_frac_part(product));

	val3 = int_to_float(85);
	val4 = int_to_float(38);
	fraction = divide_fixed_point(val3, val4);
	// 85 / 38 = 2.2368421052631578947368421052632
	printf("85/38 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));	
	
	val4 |= 0xC000;
	fraction = divide_fixed_point(val3, val4);
	// 85 / 38.75 = 2.1935483870967741935483870967742
	printf("85/38.75 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	val4 = 0x6000; // 0.375
	fraction = divide_fixed_point(val3, val4);
	// 85 / 0.375 = 226.666666666666666
	printf("85/0.3125 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	val4 = int_to_float(155);
	fraction = divide_fixed_point(val3, val4);
	// 85 / 155 = 0.54838709677419354838709677419355
	printf("85/155 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	val4 = int_to_float(1550);
	fraction = divide_fixed_point(val3, val4);
	// 85 / 1550 = 0.054838709677419354838709677419355
	printf("85/1550 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	val4 = int_to_float(1);
	fraction = divide_fixed_point(val3, val4);	
	printf("85/1 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	val4 = int_to_float(3);
	fraction = divide_fixed_point(val3, val4);

	 vv = val3 / 3;
	// 85 / 4 = 41.25
	printf("85/3 = Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));
	//printf("85/3 = Fixed point value %d.%04u\n", get_int_part(vv), get_frac_part(vv));

	fraction = 0x23CA1;
	printf("Fixed point value %d.%04u\n", get_int_part(fraction), get_frac_part(fraction));

	return 0;
}
