/*
 * sd_utils.h
 *
 * Created: 30.09.2020 12:46:06
 *  Author: hanso
 */ 


#ifndef SD_UTILS_H_
#define SD_UTILS_H_

uint8_t sd_assemble_r1_rsp(uint8_t *sd_return_data, uint8_t array_size);

uint8_t * sd_assemble_r7_rsp(uint8_t *sd_return_data, uint8_t array_size);

uint8_t * sd_assemble_r3_rsp(uint8_t *sd_return_data, uint8_t array_size);

void sd_display_error_message(uint8_t error_id);

void split_32bit_val(uint32_t data, uint8_t *return_array);	// Takes in a 32bit value, and returns said value in 4 8bit chunks

#endif /* SD_UTILS_H_ */