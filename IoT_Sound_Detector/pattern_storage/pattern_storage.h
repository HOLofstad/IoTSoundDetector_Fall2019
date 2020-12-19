/*
 * pattern_storage.h
 *
 * Created: 13.10.2020 15:23:41
 *  Author: hanso
 */ 


#ifndef PATTERN_STORAGE_H_
#define PATTERN_STORAGE_H_

#define PATTERN_LENGTH	112

typedef struct StorageInfo
{
	uint16_t n_patterns_stored;
}StorageInfo;

typedef struct AudioPattern
{
	uint8_t data[PATTERN_LENGTH];
	int16_t detection_threshold;		
}AudioPattern;	// Should include a slot for Pattern.name here in the future


uint8_t store_pattern(AudioPattern pattern);

AudioPattern retrieve_pattern(uint16_t pattern_index, uint8_t *error_tracker); // Pattern index is one-indexed

uint8_t delete_patterns();

StorageInfo retrieve_storage_info(uint8_t *error_tracker);

uint8_t update_storage_info(uint8_t n_patterns_bool, StorageInfo storage_info); // Bool-values determine if a parameter gets incremented, or decremented

void record_pattern(void);

int16_t determine_detection_threshold(AudioPattern pattern);



#endif /* PATTERN_STORAGE_H_ */