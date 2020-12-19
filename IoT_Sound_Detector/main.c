#include "include/initialize.h"
#include <string.h>
#include <stdio.h>
#include "pattern_storage/pattern_storage.h"
#include "pattern_detection/pattern_detection.h"
#include "audio_sampler/audio_sampler.h"


int main(void)
{
	initialize();
	
	uint8_t record[PATTERN_LENGTH] = {0xff};
	uint8_t r_end = 0;
	int16_t detection_score = -20000;
	StorageInfo storage_info;
	uint8_t error_tracker = 0xff;
	AudioPattern pattern;
	int16_t *sampled_sig_ptr;
	
	while (1)
	{		
		// Retrieve info about patterns stored on SDC.
		// E.g. number of stored audio patterns
		storage_info = retrieve_storage_info(&error_tracker);	
		
		// On-switch for the audio_sampler module
		start_sampling();
		
		// Returns pointer to the sampled signal array, containing 128 raw samples
		sampled_sig_ptr = return_sampled_signal();
		
		// Executes frequency analysis on the sampled signal,
		// and returns the dominant frequency bin of said signal
		record[r_end] = get_main_freq(sampled_sig_ptr);	

		r_end++;	// Iterator for the record array
		
		// If the iterator exceeds the size of the record array, reset it to zero
		if (r_end >= sizeof(record))	
		{
			r_end = 0;
		}
		
		// Run through all patterns stored on SD-card
		for (uint8_t pattern_ind = 1; pattern_ind <= storage_info.n_patterns_stored; pattern_ind++)	
		{			
			// Fetch pattern from SD-card
			pattern = retrieve_pattern(pattern_ind, &error_tracker);	
			
			// Evaluate to what degree the recorded pattern matches the stored pattern from the SD-card
			detection_score = evaluate_pattern_match(record, pattern.data);	
			
			// Check for pattern match
			if (detection_score >= pattern.detection_threshold)		
			{
				printf("PATTERN %d DETECTED!\n\r", pattern_ind);
				printf("Detection value: %d\n\r", detection_score);
				
				// When a pattern is detected, reset the recorded signal
				memset(record, 0xff, PATTERN_LENGTH);		
				r_end = 0;
				detection_score = -20000;
			}			
		}
	}
}

//uint16_t timer_start = TCB0.CNT;
//uint16_t timer_end = TCB0.CNT;
//uint16_t execution_time = timer_end - timer_start;
//printf("Execution time: %d\t", execution_time);
