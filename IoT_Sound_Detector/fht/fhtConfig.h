/************************************************************************
	fhtConfig.h

    16-Bit Fast Hartley Transform
	Configuration for the FHT and associated functions
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

#ifndef FHTCONFIG_H_
#define FHTCONFIG_H_

// The FHT length is the number of input datum to the FHT (i.e. the number of samples)
// this is defined by FHT_SCALE which denotes 2^6 = 64 where FHT_SCALE = 6
// You must define both the FHT_LEN and FHT_SCALE values to match according to the
// following table:
//
// FHT_SCALE | FHT_LEN
//     7     |  128

// Define the required FHT length here (i.e. the number of input samples).
// Valid values are 128.
#define FHT_LEN		128
#define FHT_SCALE	7

// Include the standard int and bool type definitions
#include <stdbool.h>
#include <stdint.h>


#include "fhtLibraryVersion.h"
#include "fht.h"
#include "complexToReal.h"

#endif /* FHTCONFIG_H_ */
