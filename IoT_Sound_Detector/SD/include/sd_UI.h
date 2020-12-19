/*
 * sd_UI.h
 *
 * Created: 01.10.2020 16:27:15
 *  Author: hanso
 */ 


#ifndef SD_UI_H_
#define SD_UI_H_

#define DATA_BLOCK_SIZE	0x200UL

uint8_t sd_write_single_block(uint16_t block_number, uint8_t *write_data, uint8_t recursive_count);

uint8_t sd_read_single_block(uint16_t block_number, uint8_t *return_array, uint8_t recursive_count);

uint8_t sd_init(uint8_t recursive_count);



#endif /* SD_UI_H_ */