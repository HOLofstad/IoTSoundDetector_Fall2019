/*
 * pattern_detection.c
 *
 * Created: 14.10.2020 15:09:51
 *  Author: hanso
 */ 

#include <stdint.h>
#include <stdlib.h>
#include "fht/fht.h"
#include "fht/peak_power.h"
#include "pattern_detection.h"

uint8_t get_main_freq(int16_t *sampled_signal)
{
	fhtDitInt(sampled_signal);
	return get_peak_power_bin(sampled_signal);
}

// Returns a score based on the similarity of the two input patterns. 
int16_t evaluate_pattern_match(uint8_t *recorded_pattern, uint8_t *reference_pattern)
{		
	// The detection variable keeps track of how certain the algorithm is of a pattern match. 
	// The higher the value, the more likely there's a match. 
	int16_t detection = 0;
	int16_t detection_top_val = -20000;
	uint8_t offset = find_initial_offset(recorded_pattern, reference_pattern);
	
	// This for-loop controls the pattern compare offset. It causes the algorithm to run 112 times for each audio pattern
	// With this loop enabled the algorithm requires ~70ms to execute for each audio pattern. 
	//for (uint8_t offset = 0; offset < RECORD_SIZE; offset++)	
	//{															
		detection = 0;
		
		for (uint8_t p_idx = 0; p_idx < RECORD_SIZE; p_idx++)	// RECORD_SIZE is set to 112
		{
			int8_t p = reference_pattern[p_idx];

			uint8_t r_idx = (offset + 1 + p_idx);
			
			// Ensures r_idx rolls over and starts at element zero whenever it exceeds the array size
			if (r_idx >= RECORD_SIZE)		
			{
				r_idx -= RECORD_SIZE;
			}
			int8_t r = recorded_pattern[r_idx];

			// Finds the difference between the recorded signal and the reference signal
			uint8_t match_distance = abs(r-p);
		
			if (p == 0 && r == 0)
			{
				// Don't award points for mutual silence
				detection += 0;
			}
			else if (match_distance == 0)
			{			
				// If the elements are equal then award 2 points in favor of detection				
				detection += 2;
			}
			else if (p != 0)
			{
				// If the elements are not equal,  
				// and the reference pattern is something other than silence, 
				// subtract 2 points from detection
				detection -= 2;		
			}
		}
		if (detection > detection_top_val)	// This if-statement serves the offset for-loop, 
		{									// and ensures that the highest detection value is returned.
			detection_top_val = detection;	// Serves no purpose when offset for-loop is disabled
		}
		
	//}
	return detection_top_val;
}


uint8_t find_initial_offset(uint8_t *recorded_pattern, uint8_t *reference_pattern)
{
	// Lookahead determines how many consecutive values must be equal to
	// be deemed a sufficient starting point
	uint8_t lookahead = 4;
	uint8_t p_idx = 0;
	int8_t p = reference_pattern[p_idx];
		
	// Search through recorded_pattern to find value equal to reference_pattern[0]
	for (uint8_t r_idx = 0; r_idx < RECORD_SIZE; r_idx++)
	{
		int8_t r = recorded_pattern[r_idx];
		uint8_t match_distance = abs(r-p);
		
		// If reference_pattern[0] == recorded_pattern[r_idx],
		// then check that the next 4 (lookahead) values are equal as well
		if (match_distance == 0)
		{
			for (uint8_t i = 0; i < lookahead; i++)
			{
				p = reference_pattern[p_idx+i];
				r = recorded_pattern[r_idx+i];
				match_distance = abs(r-p);
				if (match_distance != 0)
				{
					// If values not equal, start over
					break;
				}
				if (i == lookahead-1)
				{
					// If all values are equal, return desired initial offset
					return r_idx;
				}
			}
		}
	}		
	return 0;
}

