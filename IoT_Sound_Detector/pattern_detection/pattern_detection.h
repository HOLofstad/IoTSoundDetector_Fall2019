/*
 * pattern_detection.h
 *
 * Created: 14.10.2020 15:10:03
 *  Author: hanso
 */ 


#ifndef PATTERN_DETECTION_H_
#define PATTERN_DETECTION_H_

#define RECORD_SIZE	112

uint8_t get_main_freq(int16_t *sampled_signal);

int16_t evaluate_pattern_match(uint8_t *recorded_pattern, uint8_t *reference_pattern);

uint8_t find_initial_offset(uint8_t *recorded_pattern, uint8_t *reference_pattern);

#endif /* PATTERN_DETECTION_H_ */