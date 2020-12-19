/*
 * pattern_storage.c
 *
 * Created: 13.10.2020 15:23:35
 *  Author: hanso
 */ 

#include <stdint.h>
#include <stdio.h>
#include "pattern_storage.h"
#include "../SD/include/sd_UI.h"
#include "../pattern_detection/pattern_detection.h"
#include "../audio_sampler/audio_sampler.h"
#include "../SD/include/sd_utils.h"

// Block number containing StorageInfo
uint8_t info_block_number = 0;

// Byte positions of specific information
uint8_t n_patterns_Bp[2] = {0, 1};
uint8_t detection_threshold_Bp[2] = {112,113}; 

uint8_t store_pattern(AudioPattern pattern)
{
	uint8_t error_tracker = 0xff;
	uint8_t data_array[DATA_BLOCK_SIZE];
	
	// Retrieve StorageInfo to determine where to store next pattern
	StorageInfo storage_info = retrieve_storage_info(&error_tracker);
	
	// If error occurred, return error
	if (error_tracker != 0xff)
	{
		return error_tracker;
	}
	
	// Pad pattern.data with 0x00 to match block size of SDC
	for (uint16_t i = 0; i < DATA_BLOCK_SIZE; i++)
	{
		data_array[i] = (i < PATTERN_LENGTH) ? pattern.data[i] : 0x00;
	}
	
	// Include detection threshold
	data_array[detection_threshold_Bp[0]] = pattern.detection_threshold >> 8;
	data_array[detection_threshold_Bp[1]] = pattern.detection_threshold & 0xff;
	
	// Determine next available data block
	uint16_t next_available_block = storage_info.n_patterns_stored + 1;
	
	// Write AudioPattern data to SDC
	error_tracker = sd_write_single_block(next_available_block, data_array, 0);
	
	if (error_tracker != 0xff)
	{
		return error_tracker;
	}
	
	// Update storage info, to reflect that a new pattern has been added
	error_tracker = update_storage_info(1, storage_info);
	
	return error_tracker;
}

// Retrieve an AudioPattern from storage
// Pattern index is one-indexed
AudioPattern retrieve_pattern(uint16_t pattern_index, uint8_t *error_tracker) 
{
	AudioPattern pattern;
	uint8_t return_data[512];
	
	// Read pattern data from SDC
	error_tracker[0] = sd_read_single_block(pattern_index, return_data, 0);
	
	// Add retrieved data to AudioPattern struct
	for (uint8_t i = 0; i < 112; i++)
	{
		pattern.data[i] = return_data[i];
 	}
	// Add detection threshold to AudioPattern struct
	pattern.detection_threshold = (return_data[detection_threshold_Bp[0]] << 8) | (return_data[detection_threshold_Bp[1]]);
	
	return pattern;
}

uint8_t delete_patterns()
{
	uint8_t erase_data[512] = {0x00};
	uint8_t error_tracker = 0xff;4
	
	// Retrieve storage info to get number of patterns stored on SDC
	StorageInfo storage_info = retrieve_storage_info(&error_tracker);
	
	// Cycle through each pattern + StorageInfo block and delete one-by-one
	for (uint16_t i = 0; i < storage_info.n_patterns_stored+1; i++)
	{
		error_tracker = sd_write_single_block(i, erase_data, 0);
		if (error_tracker != 0xff)
		{
			return error_tracker;
		}
	}
	return error_tracker;
}

StorageInfo retrieve_storage_info(uint8_t *error_tracker)
{
	StorageInfo storage_info;
	
	uint8_t return_data[DATA_BLOCK_SIZE];
	
	error_tracker[0] = sd_read_single_block(info_block_number, return_data, 0);
	
	storage_info.n_patterns_stored = (return_data[n_patterns_Bp[0]] << 8) | (return_data[n_patterns_Bp[1]]);
	
	return storage_info;
}

// Update StorageInfo info block in SDC
// n_patterns_bool determine if n_patterns_stored gets incremented, or decremented
// storage_info should include current data in StorageInfo block
uint8_t update_storage_info(uint8_t n_patterns_bool, StorageInfo storage_info) 
{	
	uint8_t error_tracker = 0xff;
	
	// if n_patterns_bool==TRUE, increment n_patterns_stored
	// Otherwise, decrement
	uint16_t updated_n_patterns =  n_patterns_bool ? storage_info.n_patterns_stored+1 : storage_info.n_patterns_stored-1;	
	
	// Construct data StorageInfo data blob
	uint8_t write_data[DATA_BLOCK_SIZE];
	write_data[n_patterns_Bp[0]] = (updated_n_patterns & 0xff00) >> 8;
	write_data[n_patterns_Bp[1]] = (updated_n_patterns & 0xff);
	
	for (uint16_t i = 2; i < DATA_BLOCK_SIZE; i++)	// Pad pattern_data with 0xff to match block size of sd-card
	{
		write_data[i] = 0x00;
	}
	
	// Write StorageInfo data blob to designated spot in SDC
	error_tracker = sd_write_single_block(info_block_number, write_data, 0);
	return error_tracker;
}

void record_pattern(void)
{
	AudioPattern pattern;
	uint8_t sd_error_tracker = 0xff;
	int16_t *sampled_sig_ptr;
	printf("Recording pattern..");
	
	// Record, analyze, and store 112 (PATTERN_LENGTH) sample batches
	for (uint8_t i = 0; i < PATTERN_LENGTH; i++)
	{
		// Get sample batch (128 samples) from audio_sampler module
		sampled_sig_ptr = return_sampled_signal();
		// Extract main frequency component of sampled signal
		pattern.data[i] = get_main_freq(sampled_sig_ptr);
	}
	
	pattern.detection_threshold = determine_detection_threshold(pattern);
	
	printf("Done recording\n\r");
	printf("Saving pattern...");
	
	// Store newly recorded pattern to SDC
	sd_error_tracker = store_pattern(pattern);
	
	// If no error, report success
	if (sd_error_tracker == 0xff)
	{
		printf("Success\n\r");
	}
	// If error occurred, report and suspend execution
	else
	{
		printf("Failed\n\r");
		sd_display_error_message(sd_error_tracker);
		while(1);	// In case of error, suspend execution (temporary feature)
	}
	
	// Print new pattern data to PC terminal (USART)
	for (uint8_t k = 0; k < 7; k++)
	{
		for (uint8_t l = 0; l < 16; l++)
		{
			printf("0x%2x, ", pattern.data[16*k+l]);
		}
		printf("\n\r");
	}
	
}

int16_t determine_detection_threshold(AudioPattern pattern)
{
	// detection_difficulty determines how strict the pattern detection should be
	uint8_t detection_difficulty = 50;
	
	// Create empty pattern to simulate a silent room
	uint8_t empty_pattern[PATTERN_LENGTH] = {0};
		
	int16_t detection_threshold = 20000;
	
	// Determine base detection score for the pattern in a silent room
	int16_t initial_detection_value = evaluate_pattern_match(empty_pattern, pattern.data);
	
	// detection_threshold is silent room score + difficulty
	// detection_difficulty can be tweaked to a suitable value
	detection_threshold = initial_detection_value + detection_difficulty;
	
	return detection_threshold;
}
