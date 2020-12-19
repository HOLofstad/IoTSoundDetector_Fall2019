/*
 * sd_driver.h
 *
 * Created: 02.09.2020 16:19:40
 *  Author: hanso
 */ 


#ifndef SD_DRIVER_H_
#define SD_DRIVER_H_

#include <stdint.h>

uint8_t sd_receive_byte();

void sd_receive_bytes(uint8_t *return_array, uint16_t n_bytes);

void sd_send_byte(uint8_t data);

void sd_send_bytes(uint8_t *data_array, uint16_t n_bytes);

void sd_send_cmd(uint8_t *cmd);

void sd_send_dummy_bytes(uint16_t n_bytes);

void sd_cs_enable();

void sd_cs_disable();


#endif /* SD_DRIVER_H_ */