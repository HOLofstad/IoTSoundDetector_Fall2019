/************************************************************************
	complextoreal.c

    16-Bit Fast Hartley Transform
	Functions to convert complex output to linear real numbers
    Copyright (C) 2013 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com (please use the forum for questions)
	Forum: http://www.waitingforfriday.com/forum

************************************************************************/

// Include configuration
#include "fhtConfig.h"

// Local includes
#include "complexToReal.h"

#include <stdlib.h>

uint8_t get_peak_idx(int16_t *fx)
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







