/*
 * sd_commands.h
 *
 * Created: 30.09.2020 12:36:39
 *  Author: hanso
 */ 


#ifndef SD_COMMANDS_H_
#define SD_COMMANDS_H_

uint8_t sd_cmd0(uint8_t *error);

uint8_t sd_cmd1(uint8_t *error);

uint8_t * sd_cmd8(uint8_t *error);

uint8_t sd_cmd16(uint32_t block_size, uint8_t *error);

void sd_cmd17(uint32_t address, uint8_t *return_array, uint8_t *error, uint16_t data_block_size);

void sd_cmd24(uint32_t address, uint8_t *data_block, uint8_t *error);

uint8_t * sd_cmd58(uint8_t *error);

uint8_t sd_cmd55(uint8_t *error);

uint8_t sd_acmd41(uint8_t *error);

#endif /* SD_COMMANDS_H_ */