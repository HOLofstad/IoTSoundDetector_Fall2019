/*
 * peak_power.c
 *
 * Created: 22.05.2018 16:09:02
 *  Author: M44001
 */
#include <avr/io.h>

#include <stdlib.h>

#include "fhtConfig.h"
#include "peak_power.h"

uint8_t get_peak_power_bin(int16_t *fx)
{
	uint16_t max_val = 0;
	uint8_t max_pos = 0;

	for (uint8_t k = 1; k < (FHT_LEN / 2); k++)
	{
		uint16_t a = abs(fx[k]);
		uint16_t b = abs(fx[FHT_LEN-k]);

		uint16_t c = a > b? a+b/2 : b+a/2;
		c = c > 1024? c : 0;

		fx[k] = c;

		if (c > max_val)
		{
			max_val = c;
			max_pos = k;
		}
	}

	return max_pos;
}
